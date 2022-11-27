#include "parser.h"
#include <memory.h>

typedef struct bits_be_reader_t {
	const uint8_t *restrict u8;
	uintptr_t size;
	uintptr_t upos;
	uint32_t cache_bits;
	uint32_t cache_value;
} bits_be_reader_t;

// bits

static bits_be_reader_t* bits_be_reader_initial(bits_be_reader_t *restrict br, const uint8_t *restrict d, uintptr_t n)
{
	br->u8 = d;
	br->size = n;
	br->upos = 0;
	br->cache_bits = 0;
	br->cache_value = 0;
	return br;
}

static uintptr_t bits_be_reader_bits_pos(bits_be_reader_t *restrict br)
{
	return br->upos * 8 - br->cache_bits;
}

// static uintptr_t bits_be_reader_bits_res(bits_be_reader_t *restrict br)
// {
// 	return (br->size - br->upos) * 8 + br->cache_bits;
// }

static uint32_t bits_be_reader_load_be_with_filter(bits_be_reader_t *restrict br, uint32_t n, uint32_t *restrict rv)
{
	const uint8_t *restrict u8;
	uintptr_t e, p;
	uint32_t mark, v, vn;
	u8 = br->u8;
	e = br->size;
	p = br->upos;
	mark = !!(p && u8[p - 1] == 0xff);
	for (v = vn = 0; vn < n && p < e; ++p)
	{
		if (!mark)
		{
			if (u8[p] != 0xff)
			{
				v = (v << 8) | u8[p];
				++vn;
			}
			else mark = 1;
		}
		else
		{
			if (u8[p] == 0x00)
			{
				v = (v << 8) | 0xff;
				++vn;
			}
			mark = 0;
		}
	}
	br->upos = p;
	if (rv) *rv = v;
	return vn;
}

static bits_be_reader_t* bits_be_reader_try_cache(bits_be_reader_t *restrict br)
{
	uint32_t v, bits;
	if ((bits = bits_be_reader_load_be_with_filter(br, 4 - ((br->cache_bits + 7) >> 3), &v) << 3))
	{
		br->cache_value |= v << br->cache_bits;
		br->cache_bits += bits;
		return br;
	}
	return NULL;
}

static bits_be_reader_t* bits_be_reader_read_bits(bits_be_reader_t *restrict br, uint32_t bits, uint32_t *restrict rvalue)
{
	uint32_t v, nb, sh;
	v = 0;
	do {
		if ((nb = (bits <= br->cache_bits)?bits:br->cache_bits))
		{
			sh = (br->cache_bits -= nb);
			v = (v << nb) | (br->cache_value >> sh);
			br->cache_value &= (1 << sh) - 1;
			bits -= nb;
		}
	} while (bits && bits_be_reader_try_cache(br));
	if (rvalue) *rvalue = v;
	return bits?NULL:br;
}

static bits_be_reader_t* bits_be_reader_read_any(bits_be_reader_t *restrict br, uint32_t *restrict rbits, uint32_t *restrict rvalue)
{
	if (br->cache_bits || bits_be_reader_try_cache(br))
	{
		if (rbits) *rbits = br->cache_bits;
		if (rvalue) *rvalue = br->cache_value;
		br->cache_bits = 0;
		br->cache_value = 0;
		return br;
	}
	return NULL;
}

static bits_be_reader_t* bits_be_reader_back_pos(bits_be_reader_t *restrict br, uint32_t bits, uint32_t value)
{
	if (bits + br->cache_bits <= 32)
	{
		if (bits < 32)
			value &= (1 << bits) - 1;
		br->cache_value |= value << br->cache_bits;
		br->cache_bits += bits;
		return br;
	}
	return NULL;
}

static bits_be_reader_t* bits_be_reader_read_huffman(bits_be_reader_t *restrict br, const huffman_decode_s *restrict hd, uint32_t *restrict rvalue)
{
	const huffman_jumper_t *restrict jp, *restrict jv;
	uintptr_t p, jn;
	uint32_t v, b;
	jp = hd->jumper;
	jn = hd->jumper_number;
	p = 0;
	if (p < jn)
	{
		while (bits_be_reader_read_any(br, &b, &v))
		{
			while (b)
			{
				b -= 1;
				jv = jp + p;
				if ((v >> b) & 1)
					p = jv->bit_1_jumpto;
				else p = jv->bit_0_jumpto;
				if (p >= jn)
					goto label_okay;
			}
		}
	}
	label_fail:
	return NULL;
	label_okay:
	if (b && !bits_be_reader_back_pos(br, b, v))
		goto label_fail;
	if (rvalue) *rvalue = (uint32_t) (((v >> b) & 1)?jv->bit_1_value:jv->bit_0_value);
	if (!~p) goto label_fail;
	return br;
}

// print

void jpeg_parser_segment__mcu_print_unit(mlog_s *restrict mlog, const int32_t unit[64])
{
	uintptr_t i, j, row, col;
	row = col = 8;
	for (i = j = 0, row *= col; i < row; ++i)
	{
		if (++j >= col) j = 0;
		mlog_printf(mlog, "%5d%s", unit[i], j?", ":"\n");
	}
}

// parse

static jpeg_parser_s* jpeg_parser_segment__mcu_parse_unit(jpeg_parser_s *restrict p, bits_be_reader_t *restrict br, int32_t unit[64], const huffman_decode_s *restrict dc, const huffman_decode_s *restrict ac)
{
	static const uintptr_t zigzag_seq2index[64] = {
		 0,  1,  8, 16,  9,  2,  3, 10,
		17, 24, 32, 25, 18, 11,  4,  5,
		12, 19, 26, 33, 40, 48, 41, 34,
		27, 20, 13,  6,  7, 14, 21, 28,
		35, 42, 49, 56, 57, 50, 43, 36,
		29, 22, 15, 23, 30, 37, 44, 51,
		58, 59, 52, 45, 38, 31, 39, 46,
		53, 60, 61, 54, 47, 55, 62, 63,
		// MCU: Zig-zag sequence of quantized DCT coefficients
		//  0  1  5  6 14 15 27 28
		//  2  4  7 13 16 26 29 42
		//  3  8 12 17 25 30 41 43
		//  9 11 18 24 31 40 44 53
		// 10 19 23 32 39 45 52 54
		// 20 22 33 38 46 51 55 60
		// 21 34 37 47 50 56 59 61
		// 35 36 48 49 57 58 62 63
	};
	uintptr_t i, n;
	uint32_t s4, r4, rs, v;
	memset(unit, 0, sizeof(*unit) * 64);
	// read dc
	if (!bits_be_reader_read_huffman(br, dc, &s4))
		goto label_fail;
	if (s4 >= 16)
		goto label_fail;
	if (!bits_be_reader_read_bits(br, s4, &v))
		goto label_fail;
	if (s4 && !((v >> (s4 - 1)) & 1))
	{
		v |= (~(uint32_t) 0) << s4;
		v += 1;
	}
	unit[zigzag_seq2index[0]] = (int32_t) v;
	// read ac[]
	i = 1;
	n = 64;
	do {
		// read zero, ac
		if (!bits_be_reader_read_huffman(br, ac, &rs))
			goto label_fail;
		s4 = rs & 15;
		r4 = rs >> 4;
		if (s4 >= 16 || r4 >= 16)
			goto label_fail;
		// skip zero
		if (r4 + i <= n)
			i += r4;
		else i = n;
		// read ac
		if (s4)
		{
			if (!bits_be_reader_read_bits(br, s4, &v))
				goto label_fail;
			if (s4 && !((v >> (s4 - 1)) & 1))
			{
				v |= (~(uint32_t) 0) << s4;
				v += 1;
			}
			if (i < n) unit[zigzag_seq2index[i++]] = (int32_t) v;
		}
	} while (i < n && rs);
	return p;
	label_fail:
	return NULL;
}

static uintptr_t jpeg_parser_segment__mcu_parse(jpeg_parser_s *restrict p, const uint8_t *restrict d, uintptr_t n)
{
	const frame_info_s *restrict info;
	const frame_scan_s *restrict scan;
	const frame_channel_t *restrict ch;
	const huffman_decode_s *dc, *ac;
	const quantization_s *q;
	bits_be_reader_t br;
	uint32_t mcu_hn, mcu_vn, hh, vv;
	uint32_t i, in, j, jn;
	int32_t unit[64];
	if ((info = p->info) && (scan = p->scan))
	{
		bits_be_reader_initial(&br, d, n);
		// calc
		mcu_hn = mcu_vn = 1;
		for (i = 0, in = scan->scan_number; i < in; ++i)
		{
			ch = frame_info_get_channel(info, scan->scan[i].channel_id);
			dc = (const huffman_decode_s *) jpeg_parser_get_table(&p->h_dc, scan->scan[i].huffman_dc_id);
			ac = (const huffman_decode_s *) jpeg_parser_get_table(&p->h_ac, scan->scan[i].huffman_ac_id);
			q = (const quantization_s *) jpeg_parser_get_table(&p->q, ch?ch->quantization_id:0);
			if (!ch || !dc || !ac || !q) goto label_fail;
			if (ch->mcu_h_number > mcu_hn) mcu_hn = ch->mcu_h_number;
			if (ch->mcu_v_number > mcu_vn) mcu_vn = ch->mcu_v_number;
		}
		mcu_hn *= 8;
		mcu_vn *= 8;
		mcu_hn = (info->width + mcu_hn - 1) / mcu_hn;
		mcu_vn = (info->height + mcu_vn - 1) / mcu_vn;
		mlog_printf(p->m, "mcu (%u, %u)\n", mcu_hn, mcu_vn);
		for (hh = 0; hh < mcu_hn; ++hh)
		{
			for (vv = 0; vv < mcu_vn; ++vv)
			{
				// mcu
				for (i = 0, in = scan->scan_number; i < in; ++i)
				{
					ch = frame_info_get_channel(info, scan->scan[i].channel_id);
					dc = (const huffman_decode_s *) jpeg_parser_get_table(&p->h_dc, scan->scan[i].huffman_dc_id);
					ac = (const huffman_decode_s *) jpeg_parser_get_table(&p->h_ac, scan->scan[i].huffman_ac_id);
					for (j = 0, jn = ch->mcu_h_number * ch->mcu_v_number; j < jn; ++j)
					{
						if (!jpeg_parser_segment__mcu_parse_unit(p, &br, unit, dc, ac))
							goto label_fail;
						mlog_printf(p->m, "mcu(%u, %u) ch(%u) %u/%u ...\n", hh, vv, scan->scan[i].channel_id, j, jn);
						tmlog_add(p->td, 1);
						jpeg_parser_segment__mcu_print_unit(p->m, unit);
						tmlog_sub(p->td, 1);
					}
				}
			}
		}
		label_fail:
		return (bits_be_reader_bits_pos(&br) + 7) >> 3;
	}
	return 0;
}

jpeg_parser_s* jpeg_parser_segment__mcu(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t, uintptr_t size)
{
	uintptr_t n;
	tmlog_add(p->td, 1);
	n = jpeg_parser_segment__mcu_parse(p, t->data + t->pos, size);
	t->pos += n;
	tmlog_sub(p->td, 1);
	return n?p:NULL;
}
