#define _DEFAULT_SOURCE
#include "parser.h"
#include <memory.h>
#include <math.h>

// bits

typedef struct bits_be_reader_t {
	const uint8_t *restrict u8;
	uintptr_t size;
	uintptr_t upos;
	uint32_t cache_bits;
	uint32_t cache_value;
} bits_be_reader_t;

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
	v = (uint32_t) (((v >> b) & 1)?jv->bit_1_value:jv->bit_0_value);
	if (rvalue) *rvalue = v;
	if (!~p) goto label_fail;
	return br;
}

// fdct8x8

typedef struct fdct8x8_s {
	double c[64];
} fdct8x8_s;

static fdct8x8_s* fdct8x8_alloc(void)
{
	fdct8x8_s *restrict r;
	uintptr_t i, j, p, n;
	if ((r = (fdct8x8_s *) refer_alloz(sizeof(fdct8x8_s))))
	{
		p = 0;
		n = 8;
		for (j = 0; j < n; ++j)
		{
			for (i = 0; i < n; ++i)
				r->c[p++] = cos((i * 2 + 1) * j * (M_PI / 16)) * (j?(1.0 / 2):(M_SQRT1_2 / 2));
		}
		return r;
	}
	return NULL;
}

// static void fdct8x8_xy2uv(const fdct8x8_s *restrict t, int32_t uv[64], const int32_t xy[64], int32_t addend)
// {
// 	uintptr_t x, y, u, v, n;
// 	uintptr_t y8, u8, v8;
// 	double c;
// 	n = 8;
// 	for (v = 0; v < n; ++v)
// 	{
// 		v8 = v << 3;
// 		for (u = 0; u < n; ++u)
// 		{
// 			u8 = u << 3;
// 			c = 0;
// 			for (y = 0; y < n; ++y)
// 			{
// 				y8 = y << 3;
// 				for (x = 0; x < n; ++x)
// 					c += (double) xy[y8 + x] * t->c[u8 + x] * t->c[v8 + y];
// 			}
// 			uv[v8 + u] = (int32_t) round(c) + addend;
// 		}
// 	}
// }

static void fdct8x8_uv2xy(const fdct8x8_s *restrict t, int32_t xy[64], const int32_t uv[64], int32_t addend, int32_t min, int32_t max)
{
	uintptr_t x, y, u, v, n;
	uintptr_t y8, v8;
	double c;
	int32_t ci;
	n = 8;
	for (y = 0; y < n; ++y)
	{
		y8 = y << 3;
		for (x = 0; x < n; ++x)
		{
			c = 0;
			for (v = 0; v < n; ++v)
			{
				v8 = v << 3;
				for (u = 0; u < n; ++u)
					c += (double) uv[v8 + u] * t->c[(u << 3) + x] * t->c[v8 + y];
			}
			ci = (int32_t) round(c) + addend;
			// if (ci < min) ci = min;
			// if (ci > max) ci = max;
			xy[y8 + x] = ci;
		}
	}
}

// mcu

typedef struct mcu_data_t {
	int32_t unit[64];
	int32_t cpixel[64];
} mcu_data_t;

typedef struct mcu_ch_s {
	uint32_t channel_id;
	uint32_t quantization_id;
	uint32_t huffman_dc_id;
	uint32_t huffman_ac_id;
	uint32_t mcu_h_number;
	uint32_t mcu_v_number;
	uint32_t unit_data_number;
	int32_t unit_data_pred;
	mcu_data_t *unit_data_array;
	const huffman_decode_s *dc;
	const huffman_decode_s *ac;
	const quantization_s *q;
} mcu_ch_s;

typedef struct mcu_s {
	const fdct8x8_s *fdct;
	uintptr_t ch_number;
	mcu_ch_s *ch_array[];
} mcu_s;

static void mcu_ch_free_func(mcu_ch_s *restrict r)
{
	if (r->dc) refer_free(r->dc);
	if (r->ac) refer_free(r->ac);
	if (r->q) refer_free(r->q);
}

static mcu_ch_s* mcu_ch_alloc(jpeg_parser_s *restrict p, const frame_channel_t *restrict ch1, const frame_scan_ch_t *restrict ch2)
{
	mcu_ch_s *restrict r;
	uint32_t unit_data_number;
	if ((unit_data_number = ch1->mcu_h_number * ch1->mcu_v_number) &&
		(r = (mcu_ch_s *) refer_alloz(sizeof(mcu_ch_s) + sizeof(mcu_data_t) * unit_data_number)))
	{
		refer_set_free(r, (refer_free_f) mcu_ch_free_func);
		r->channel_id = ch1->channel_id;
		r->quantization_id = ch1->quantization_id;
		r->huffman_dc_id = ch2->huffman_dc_id;
		r->huffman_ac_id = ch2->huffman_ac_id;
		r->mcu_h_number = ch1->mcu_h_number;
		r->mcu_v_number = ch1->mcu_v_number;
		r->unit_data_number = unit_data_number;
		r->unit_data_pred = 0;
		r->unit_data_array = (mcu_data_t *) (r + 1);
		r->dc = (const huffman_decode_s *) refer_save(jpeg_parser_get_table(&p->h_dc, r->huffman_dc_id));
		r->ac = (const huffman_decode_s *) refer_save(jpeg_parser_get_table(&p->h_ac, r->huffman_ac_id));
		r->q = (const quantization_s *) refer_save(jpeg_parser_get_table(&p->q, r->quantization_id));
		if (r->dc && r->ac && r->q)
			return r;
		refer_free(r);
	}
	return NULL;
}

static void mcu_free_func(mcu_s *restrict r)
{
	uintptr_t i, n;
	if (r->fdct) refer_free(r->fdct);
	for (i = 0, n = r->ch_number; i < n; ++i)
	{
		if (r->ch_array[i])
			refer_free(r->ch_array[i]);
	}
}

static mcu_s* mcu_alloc(jpeg_parser_s *restrict p, const frame_info_s *restrict info, const frame_scan_s *restrict scan)
{
	const frame_channel_t *restrict ch;
	mcu_s *restrict r;
	uintptr_t i, n;
	if ((n = (uintptr_t) scan->scan_number) &&
		(r = (mcu_s *) refer_alloz(sizeof(mcu_s) + sizeof(mcu_ch_s *) * n)))
	{
		refer_set_free(r, (refer_free_f) mcu_free_func);
		r->ch_number = n;
		for (i = 0; i < n; ++i)
		{
			if (!(ch = frame_info_get_channel(info, scan->scan[i].channel_id)))
				goto label_fail;
			if (!(r->ch_array[i] = mcu_ch_alloc(p, ch, scan->scan + i)))
				goto label_fail;
		}
		if ((r->fdct = fdct8x8_alloc()))
			return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static int32_t mcu_get_ch_pixel(const mcu_s *restrict mcu, uintptr_t ch_index, uint32_t x, uint32_t y)
{
	const mcu_ch_s *restrict ch;
	if (ch_index < mcu->ch_number)
	{
		ch = mcu->ch_array[ch_index];
		return ch->unit_data_array[
			((y >> 3) % ch->mcu_v_number) * ch->mcu_h_number + ((x >> 3) % ch->mcu_h_number)].
			cpixel[((y & 7) << 3) | (x & 7)];
	}
	return 0;
}

// display

static void jpeg_parser_segment__mcu_display(display_s *restrict d, uint32_t px, uint32_t py, mcu_s *restrict mcu, uint32_t w, uint32_t h)
{
	uint8_t *restrict bgra;
	uintptr_t x, y;
	uint32_t kyy_x, kyy_y, kcb_x, kcb_y, kcr_x, kcr_y;
	float yy, cb, cr, bb, gg, rr;
	if (mcu->ch_number == 3)
	{
		kyy_x = w / 8 / mcu->ch_array[0]->mcu_h_number;
		kyy_y = h / 8 / mcu->ch_array[0]->mcu_v_number;
		kcb_x = w / 8 / mcu->ch_array[1]->mcu_h_number;
		kcb_y = h / 8 / mcu->ch_array[1]->mcu_v_number;
		kcr_x = w / 8 / mcu->ch_array[2]->mcu_h_number;
		kcr_y = h / 8 / mcu->ch_array[2]->mcu_v_number;
		if ((bgra = (uint8_t *) xwindow_image_map(d->image, w, h)))
		{
			for (y = 0; y < h; ++y)
			{
				for (x = 0; x < w; ++x)
				{
					yy = (float) mcu_get_ch_pixel(mcu, 0, x / kyy_x, y / kyy_y);
					cb = (float) (mcu_get_ch_pixel(mcu, 1, x / kcb_x, y / kcb_y) - 128);
					cr = (float) (mcu_get_ch_pixel(mcu, 2, x / kcr_x, y / kcr_y) - 128);
					bb = yy + 1.772f * cb;
					gg = yy - 0.34414f * cb - 0.71414f * cr;
					rr = yy + 1.402f * cr;
					if (bb > 255) bb = 255;
					if (bb < 0) bb = 0;
					if (gg > 255) gg = 255;
					if (gg < 0) gg = 0;
					if (rr > 255) rr = 255;
					if (rr < 0) rr = 0;
					*bgra++ = (uint8_t) bb;
					*bgra++ = (uint8_t) gg;
					*bgra++ = (uint8_t) rr;
					++bgra;
				}
			}
			xwindow_image_update_full(d->image, px, py);
		}
	}
}

// print

// static void jpeg_parser_segment__mcu_print_unit(mlog_s *restrict mlog, const int32_t unit[64])
// {
// 	uintptr_t i, j, row, col;
// 	row = col = 8;
// 	for (i = j = 0, row *= col; i < row; ++i)
// 	{
// 		if (++j >= col) j = 0;
// 		mlog_printf(mlog, "%5d%s", unit[i], j?", ":"\n");
// 	}
// }

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
		i += r4;
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
			if (i < n) unit[zigzag_seq2index[i]] = (int32_t) v;
		}
		i += 1;
	} while (i < n && rs);
	return p;
	label_fail:
	return NULL;
}

static void jpeg_parser_segment__mcu_unit_mul_q(int32_t unit[64], const quantization_s *restrict q)
{
	uintptr_t i, n;
	for (i = 0, n = 64; i < n; ++i)
		unit[i] *= q->q[i];
}

static uintptr_t jpeg_parser_segment__mcu_parse(jpeg_parser_s *restrict p, const uint8_t *restrict d, uintptr_t n)
{
	const frame_info_s *restrict info;
	display_s *restrict display;
	mcu_s *restrict mcu;
	mcu_ch_s *restrict ch;
	mcu_data_t *restrict md;
	bits_be_reader_t br;
	uint32_t mcu_w, mcu_h, mcu_hn, mcu_vn, hh, vv;
	uint32_t i, in, j, jn;
	if ((info = p->info) && p->scan &&
		(display = jpeg_parser_create_display(p, info->width, info->height)) &&
		(mcu = mcu_alloc(p, info, p->scan)))
	{
		bits_be_reader_initial(&br, d, n);
		// calc
		mcu_w = mcu_h = 1;
		for (i = 0, in = mcu->ch_number; i < in; ++i)
		{
			ch = mcu->ch_array[i];
			if (ch->mcu_h_number > mcu_w) mcu_w = ch->mcu_h_number;
			if (ch->mcu_v_number > mcu_h) mcu_h = ch->mcu_v_number;
		}
		mcu_w *= 8;
		mcu_h *= 8;
		mcu_hn = (info->width + mcu_w - 1) / mcu_w;
		mcu_vn = (info->height + mcu_h - 1) / mcu_h;
		mlog_printf(p->m, "mcu n(%u, %u) x size(%u, %u)\n", mcu_hn, mcu_vn, mcu_w, mcu_h);
		for (vv = 0; vv < mcu_vn; ++vv)
		{
			for (hh = 0; hh < mcu_hn; ++hh)
			{
				// load mcu
				for (i = 0, in = mcu->ch_number; i < in; ++i)
				{
					ch = mcu->ch_array[i];
					md = ch->unit_data_array;
					for (j = 0, jn = ch->unit_data_number; j < jn; ++j)
					{
						if (!jpeg_parser_segment__mcu_parse_unit(p, &br, md[j].unit, ch->dc, ch->ac))
							goto label_fail;
						ch->unit_data_pred = (md[j].unit[0] += ch->unit_data_pred);
						jpeg_parser_segment__mcu_unit_mul_q(md[j].unit, ch->q);
						fdct8x8_uv2xy(mcu->fdct, md[j].cpixel, md[j].unit, 1 << (info->sample_bits - 1), 0, (1 << info->sample_bits) - 1);
					}
				}
				// display mcu
				jpeg_parser_segment__mcu_display(display, mcu_w * hh, mcu_h * vv, mcu, mcu_w, mcu_h);
			}
		}
		label_fail:
		refer_free(mcu);
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
