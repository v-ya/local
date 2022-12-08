#include "codec.jpeg.h"

static uintptr_t mi_jpeg_decode_parse_huffman(struct media_huffman_decode_s *restrict huffman, media_huffman_index_t hi, const uint8_t *restrict h, uintptr_t n)
{
	uintptr_t index, number, pos, end;
	number = 16;
	if (number <= n)
	{
		pos = end = number;
		while (number && !h[number - 1]) --number;
		for (index = 0; index < number; ++index)
		{
			if ((end += (uintptr_t) h[index]) > n)
				goto label_fail;
			if (!media_huffman_decode_add_bits(huffman, hi))
				goto label_fail;
			while (pos < end)
			{
				if (!media_huffman_decode_add_value(huffman, hi, h + pos))
					goto label_fail;
				++pos;
			}
		}
		return pos;
	}
	label_fail:
	return 0;
}

static struct media_huffman_decode_s* mi_jpeg_decode_initial_huffman(struct media_huffman_decode_s *restrict huffman, const uint8_t *restrict data, uintptr_t size, media_huffman_index_t number)
{
	media_huffman_index_t index;
	uintptr_t length;
	for (index = 0; index < number; ++index)
	{
		if (!(length = mi_jpeg_decode_parse_huffman(huffman, index, data, size)))
			goto label_fail;
		data += length;
		size -= length;
	}
	if (!size) return huffman;
	label_fail:
	return NULL;
}

static void mi_jpeg_decode_free_func(struct mi_jpeg_decode_s *restrict r)
{
	if (r->fdct8x8) refer_free(r->fdct8x8);
	if (r->zigzag8x8) refer_free(r->zigzag8x8);
	if (r->huffman) refer_free(r->huffman);
}

struct mi_jpeg_decode_s* mi_jpeg_decode_alloc(const struct mi_jpeg_decode_param_t *restrict param)
{
	struct mi_jpeg_decode_s *restrict r;
	const struct mi_jpeg_frame_t *restrict f;
	struct media_huffman_decode_s *restrict huffman;
	struct mi_jpeg_codec_i8x8_t *restrict data;
	uintptr_t i, n, nn, qsize;
	uintptr_t mcu_w_number;
	uintptr_t mcu_h_number;
	uintptr_t mcu_unit_number;
	uintptr_t mcu_all_number;
	uint32_t mcu_w_max;
	uint32_t mcu_h_max;
	r = NULL;
	if (param->f_size < sizeof(*f))
		goto label_fail;
	f = (const struct mi_jpeg_frame_t *) param->f;
	if (f->channel * sizeof(*f->ch) < param->f_size - sizeof(*f))
		goto label_fail;
	n = (uintptr_t) f->channel;
	mcu_w_max = mcu_h_max = 1;
	for (i = 0; i < n; ++i)
	{
		if (f->ch[i].mcu_nh > mcu_w_max)
			mcu_w_max = f->ch[i].mcu_nh;
		if (f->ch[i].mcu_nv > mcu_h_max)
			mcu_h_max = f->ch[i].mcu_nv;
	}
	mcu_w_number = ((f->width + (mcu_w_max << 3) - 1) / mcu_w_max) >> 3;
	mcu_h_number = ((f->height + (mcu_h_max << 3) - 1) / mcu_h_max) >> 3;
	mcu_unit_number = 0;
	for (i = 0; i < n; ++i)
	{
		if (!f->ch[i].mcu_nh || (mcu_w_max % f->ch[i].mcu_nh))
			goto label_fail;
		if (!f->ch[i].mcu_nv || (mcu_h_max % f->ch[i].mcu_nv))
			goto label_fail;
		mcu_unit_number += (uintptr_t) f->ch[i].mcu_nh * f->ch[i].mcu_nv;
	}
	mcu_all_number = mcu_unit_number * mcu_w_number * mcu_h_number;
	nn = sizeof(struct mi_jpeg_decode_s) + sizeof(struct mi_jpeg_decode_ch_t) * n +
		sizeof(struct mi_jpeg_codec_i8x8_t) * mcu_all_number;
	if (!(r = (struct mi_jpeg_decode_s *) refer_alloc(nn)))
		goto label_fail;
	r->fdct8x8 = (const struct media_fdct_2d_i32_s *) refer_save(param->fdct8x8);
	r->zigzag8x8 = (const struct media_zigzag_s *) refer_save(param->zigzag8x8);
	r->huffman = huffman = media_huffman_decode_alloc((media_huffman_index_t) f->h_number);
	r->mcu_w_max = (uintptr_t) mcu_w_max;
	r->mcu_h_max = (uintptr_t) mcu_h_max;
	r->mcu_w_number = mcu_w_number;
	r->mcu_h_number = mcu_h_number;
	r->mcu_unit_number = mcu_unit_number;
	r->mcu_all_number = mcu_all_number;
	r->ch_number = n;
	refer_set_free(r, (refer_free_f) mi_jpeg_decode_free_func);
	if (!r->fdct8x8 || !r->zigzag8x8 || !r->huffman)
		goto label_fail;
	qsize = sizeof(*data) / sizeof(*data->v) * sizeof(*r->ch[i].q);
	if (f->q_number * qsize != param->q_size)
		goto label_fail;
	if (!mi_jpeg_decode_initial_huffman(huffman, param->h, param->h_size, (media_huffman_index_t) f->h_number))
		goto label_fail;
	data = (struct mi_jpeg_codec_i8x8_t *) ((struct mi_jpeg_decode_ch_t *) (r + 1) + n);
	nn = mcu_w_number * mcu_h_number;
	for (i = 0; i < n; ++i)
	{
		r->ch[i].mcu_npw = (uintptr_t) f->ch[i].mcu_nh;
		r->ch[i].mcu_nph = (uintptr_t) f->ch[i].mcu_nv;
		r->ch[i].mcu_npm = r->ch[i].mcu_npw * r->ch[i].mcu_nph;
		r->ch[i].depth_bits = f->depth;
		if (f->ch[i].q_index >= f->q_number)
			goto label_fail;
		r->ch[i].q = (const uint32_t *) (param->q + f->ch[i].q_index * qsize);
		if (f->ch[i].hdc_index >= f->h_number)
			goto label_fail;
		if (f->ch[i].hac_index >= f->h_number)
			goto label_fail;
		r->ch[i].hdc_index = (media_huffman_index_t) f->ch[i].hdc_index;
		r->ch[i].hac_index = (media_huffman_index_t) f->ch[i].hac_index;
		r->ch[i].data = data;
		data += r->ch[i].mcu_npm * nn;
	}
	return r;
	label_fail:
	if (r) refer_free(r);
	return NULL;
}

static inline struct media_bits_reader_t* mi_jpeg_decode_load_unit_si(struct media_bits_reader_t *restrict reader, media_bits_t s4, int32_t *restrict rv)
{
	media_bits_t v;
	if (!media_bits_reader_read_bits(reader, s4, &v))
		goto label_fail;
	if (s4 && !((v >> (uint32_t) (s4 - 1)) & 1))
	{
		v |= (~(media_bits_t) 0) << s4;
		v += 1;
	}
	*rv = (int32_t) v;
	return reader;
	label_fail:
	return NULL;
}

static struct mi_jpeg_decode_s* mi_jpeg_decode_load_unit(struct mi_jpeg_decode_s *restrict jd, struct media_bits_reader_t *restrict reader, const struct mi_jpeg_decode_ch_t *restrict ch, struct mi_jpeg_codec_i8x8_t *restrict data)
{
	const uintptr_t *restrict s2i;
	const uint8_t *restrict hv;
	uintptr_t i, n;
	media_bits_t s4, r4, rs;
	i = 0;
	n = 64;
	s2i = jd->zigzag8x8->s2i;
	if (!(hv = media_huffman_decode_bits(jd->huffman, ch->hdc_index, reader)))
		goto label_fail;
	if ((s4 = (media_bits_t) *hv) >= 16)
		goto label_fail;
	if (!mi_jpeg_decode_load_unit_si(reader, s4, &data->v[s2i[i++]]))
		goto label_fail;
	// read ac[]
	do {
		// read zero, ac
		if (!(hv = media_huffman_decode_bits(jd->huffman, ch->hac_index, reader)))
			goto label_fail;
		if (!(rs = (media_bits_t) *hv))
			break;
		s4 = rs & 15;
		r4 = rs >> 4;
		if (s4 >= 16 || r4 >= 16)
			goto label_fail;
		// skip zero
		if ((r4 += i) >= n)
			goto label_fail;
		while (i < r4)
			data->v[s2i[i++]] = 0;
		// read ac
		if (!mi_jpeg_decode_load_unit_si(reader, s4, &data->v[s2i[i++]]))
			goto label_fail;
	} while (i < n && rs);
	while (i < n) data->v[s2i[i++]] = 0;
	return jd;
	label_fail:
	return NULL;
}

struct mi_jpeg_decode_s* mi_jpeg_decode_load(struct mi_jpeg_decode_s *restrict jd, struct media_bits_reader_t *restrict reader)
{
	const struct mi_jpeg_decode_ch_t *restrict ch;
	struct mi_jpeg_codec_i8x8_t *restrict data;
	uintptr_t i, n, j, m, k, l;
	int32_t last_dc;
	n = jd->mcu_w_number * jd->mcu_h_number;
	m = jd->ch_number;
	// load unit
	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < m; ++j)
		{
			ch = jd->ch + j;
			data = ch->data + ((l = ch->mcu_npm) * i);
			for (k = 0; k < l; ++k)
			{
				if (!mi_jpeg_decode_load_unit(jd, reader, ch, data + k))
					goto label_fail;
			}
		}
	}
	// fix dc diff
	if (n)
	{
		for (j = 0; j < m; ++j)
		{
			ch = jd->ch + j;
			data = ch->data;
			l = ch->mcu_npm * n;
			last_dc = data->v[0];
			for (k = 1; k < l; ++k)
				last_dc = (data[k].v[0] += last_dc);
		}
	}
	return jd;
	label_fail:
	return NULL;
}
