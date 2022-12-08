#include "transform.jpeg.h"
#include "codec.jpeg.h"

struct media_transform_param__jpeg_parse_t {
	const struct media_s *media;
	struct mi_jpeg_decode_s *jd;
	const uint8_t *data;
	uintptr_t size;
};

struct media_transform_param__jpeg_idct_t {
	struct mi_jpeg_decode_s *jd;
	struct media_frame_s *df;
	uintptr_t mcu_index;
	uintptr_t mcu_number;
};

static const uint8_t* media_bits_reader_next_bytes__jpeg(struct media_bits_reader_t *restrict reader, uintptr_t *restrict nbyte)
{
	const uint8_t *restrict u8;
	uintptr_t p, n, i;
	u8 = reader->u8;
	p = reader->upos;
	n = reader->size;
	if (p < n)
	{
		while (u8[p] == mi_jpeg_segment_type__mark)
		{
			if (++p >= n) break;
			if (!u8[p]) goto label_ff00;
			if (++p >= n) break;
		}
		u8 += p;
		if ((n -= p) > *nbyte)
			n = *nbyte;
		for (i = 0; i < n; ++i)
		{
			if (u8[i] == mi_jpeg_segment_type__mark)
				break;
		}
		reader->upos = p + i;
		*nbyte = i;
		return u8;
	}
	return NULL;
	label_ff00:
	*nbyte = 1;
	reader->upos = p + 1;
	return u8 + p - 1;
}

static d_media_runtime__deal(jpeg_parse, struct media_transform_param__jpeg_parse_t, refer_t)
{
	struct media_bits_reader_t reader;
	if (media_bits_reader_initial_be(&reader, param->data, param->size, media_bits_reader_next_bytes__jpeg))
	{
		// media_verbose(param->media, "[deal] jpeg_parse ...");
		if (mi_jpeg_decode_load(param->jd, &reader))
			return param;
	}
	return NULL;
}

static d_media_runtime__emit(jpeg_parse)
{
	const struct media_frame_s *restrict sf;
	struct media_transform_param__jpeg_parse_t param;
	struct media_runtime_unit_param_t up;
	sf = (const struct media_frame_s *) pri_data[0];
	param.media = (const struct media_s *) pri_data[2];
	param.jd = (struct mi_jpeg_decode_s *) pri_data[1];
	param.data = sf->channel_chip[3]->data;
	param.size = sf->channel_chip[3]->size;
	up.runtime = rt;
	up.context = uc;
	up.deal = (media_runtime_deal_f) media_runtime_symbol(deal, jpeg_parse);
	up.param_size = sizeof(param);
	if (media_runtime_post_unit(&up, &param, NULL))
		return task;
	return NULL;
}

static void media_transform_inner_jpeg_idct_d8(const struct mi_jpeg_codec_i8x8_t *restrict src, const struct media_fdct_2d_i32_s *fdct8x8, uint8_t *restrict dst, const struct mi_jpeg_decode_ch_t *restrict ch, uintptr_t x, uintptr_t y)
{
	struct mi_jpeg_codec_i8x8_t pixel;
	const int32_t *restrict p;
	uint8_t *restrict q;
	uintptr_t w, h, sp, dp;
	int32_t pixel_add, min, max, v;
	if (x < ch->width && y < ch->height)
	{
		media_fdct_2d_i32_idct(fdct8x8, pixel.v, src->v);
		w = h = sp = 8;
		if (x + w > ch->width) w = ch->width - x;
		if (y + h > ch->height) h = ch->height - y;
		sp -= w;
		dp = x;
		pixel_add = (int32_t) 1 << (uint32_t) (ch->depth_bits - 1);
		min = 0;
		max = ((int32_t) 1 << (uint32_t) ch->depth_bits) - 1;
		p = pixel.v;
		q = dst + (y * ch->width + x);
		for (y = 0; y < h; ++y)
		{
			for (x = 0; x < w; ++x)
			{
				v = (*p++) + pixel_add;
				if (v < min) v = min;
				if (v > max) v = max;
				*q++ = (uint8_t) v;
			}
			p += sp;
			q += dp;
		}
	}
}

static d_media_runtime__deal(jpeg_idct_d8, struct media_transform_param__jpeg_idct_t, refer_t)
{
	struct mi_jpeg_decode_s *restrict jd;
	const struct mi_jpeg_decode_ch_t *restrict ch;
	const struct mi_jpeg_codec_i8x8_t *restrict d;
	uint8_t *restrict channel_dst;
	uintptr_t mcu_x, mcu_y, i, n, c, cn, xx, yy;
	jd = param->jd;
	i = param->mcu_index;
	mcu_y = i / jd->mcu_w_number;
	mcu_x = i % jd->mcu_w_number;
	cn = jd->ch_number;
	for (n = i + param->mcu_number; i < n; ++i)
	{
		for (c = 0; c < cn; ++c)
		{
			ch = jd->ch + c;
			d = ch->data + i * ch->mcu_npm;
			channel_dst = (uint8_t *) param->df->channel_chip[c]->data;
			for (yy = 0; yy < ch->mcu_nph; ++yy)
			{
				for (xx = 0; xx < ch->mcu_npw; ++xx)
				{
					media_transform_inner_jpeg_idct_d8(
						d, jd->fdct8x8, channel_dst, ch,
						mcu_x * ch->mcu_npw + xx,
						mcu_y * ch->mcu_nph + yy);
					++d;
				}
			}
		}
		if (++mcu_x >= jd->mcu_w_number)
		{
			mcu_x -= jd->mcu_w_number;
			mcu_y += 1;
		}
	}
	return param;
}

static d_media_runtime__emit(jpeg_idct_d8)
{
	struct mi_jpeg_decode_s *restrict jd;
	struct media_frame_s *restrict df;
	struct media_transform_param__jpeg_idct_t param;
	struct media_runtime_unit_param_t up;
	uintptr_t i, n;
	jd = (struct mi_jpeg_decode_s *) pri_data[0];
	df = (struct media_frame_s *) pri_data[1];
	if (!media_frame_set_dimension(df, 2, (const uintptr_t []) {jd->width, jd->height}))
		goto label_fail;
	if (!media_frame_touch_data(df))
		goto label_fail;
	if ((n = jd->ch_number) == df->channel)
	{
		for (i = 0; i < n; ++i)
		{
			if (jd->ch[i].width * jd->ch[i].height != df->channel_chip[i]->size)
				goto label_fail;
		}
	}
	// post ...
	param.jd = jd;
	param.df = df;
	param.mcu_index = 0;
	param.mcu_number = 0;
	up.runtime = rt;
	up.context = uc;
	up.deal = (media_runtime_deal_f) media_runtime_symbol(deal, jpeg_idct_d8);
	up.param_size = sizeof(param);
	i = media_runtime_unit_core_number(rt);
	n = jd->mcu_w_number * jd->mcu_h_number;
	i = (n + i - 1) / i;
	while (n && task)
	{
		param.mcu_number = i = (i <= n)?i:n;
		if (!media_runtime_post_unit(&up, &param, NULL))
			task = NULL;
		param.mcu_index += i;
		n -= i;
	}
	return task;
	label_fail:
	return NULL;
}

static d_media_transform__task_append(jpeg)
{
	struct mi_jpeg_decode_s *restrict jd;
	struct mi_jpeg_decode_param_t param;
	param.fdct8x8 = ((const struct media_transform_id_jpeg_s *) tf->id)->fdct8x8;
	param.zigzag8x8 = ((const struct media_transform_id_jpeg_s *) tf->id)->zigzag8x8;
	param.f = (const uint8_t *) sf->channel_chip[0]->data;
	param.q = (const uint8_t *) sf->channel_chip[1]->data;
	param.h = (const uint8_t *) sf->channel_chip[2]->data;
	param.f_size = sf->channel_chip[0]->size;
	param.q_size = sf->channel_chip[1]->size;
	param.h_size = sf->channel_chip[2]->size;
	if ((jd = mi_jpeg_decode_alloc(&param)))
	{
		struct media_runtime_task_step_t steps[] = {
			{
				.emit = media_runtime_symbol(emit, jpeg_parse),
				.pri_number = 3,
				.pri_data = (const refer_t []) {sf, jd, tf->media}
			},
			{
				.emit = media_runtime_symbol(emit, jpeg_idct_d8),
				.pri_number = 2,
				.pri_data = (const refer_t []) {jd, df}
			}
		};
		list = media_runtime_task_list_append(list, steps, sizeof(steps) / sizeof(*steps));
		refer_free(jd);
		return list;
	}
	return NULL;
}

static void media_transform_id_jpeg_free_func(struct media_transform_id_jpeg_s *restrict r)
{
	if (r->fdct8x8) refer_free(r->fdct8x8);
	if (r->zigzag8x8) refer_free(r->zigzag8x8);
	media_transform_id_free_func(&r->transform);
}

static struct media_transform_id_s* media_transform_create_image__jpeg(const struct media_s *restrict media, const char *restrict src_frame_id, const char *restrict dst_frame_id)
{
	struct media_transform_id_jpeg_s *restrict r;
	struct media_transform_id_func_t func = {
		.initial_judge = NULL,
		.create_pri = NULL,
		.open_codec = NULL,
		.dst_frame = NULL,
		.task_append = media_transform_symbol(task_append, jpeg),
	};
	if ((r = (struct media_transform_id_jpeg_s *) media_transform_id_alloc(sizeof(struct media_transform_id_jpeg_s), media, src_frame_id, dst_frame_id, &func)))
	{
		refer_set_free(r, (refer_free_f) media_transform_id_jpeg_free_func);
		if ((r->fdct8x8 = media_save_component(media, media_cp_fdct_i32_r8p16, struct media_fdct_2d_i32_s)) &&
			(r->zigzag8x8 = media_save_component(media, media_cp_zigzag_8x8, struct media_zigzag_s)))
			return &r->transform;
		refer_free(r);
	}
	return NULL;
}

struct media_transform_id_s* media_transform_create_image__jpeg_yuv_8_411(const struct media_s *restrict media)
{
	return media_transform_create_image__jpeg(media, media_nf_jpeg_yuv_8_411, media_nf_yuv_8_411);
}

struct media_transform_id_s* media_transform_create_image__jpeg_yuv_8_111(const struct media_s *restrict media)
{
	return media_transform_create_image__jpeg(media, media_nf_jpeg_yuv_8_111, media_nf_yuv_8_111);
}
