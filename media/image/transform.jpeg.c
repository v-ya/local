#include "transform.jpeg.h"
#include "codec.jpeg.h"

struct media_transform_param__jpeg_parse_t {
	struct mi_jpeg_decode_s *jd;
	const uint8_t *data;
	uintptr_t size;
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

static d_media_runtime__deal(jpeg_parse, struct media_transform_param__jpeg_parse_t, const struct media_s *restrict)
{
	struct media_bits_reader_t reader;
	if (media_bits_reader_initial_be(&reader, param->data, param->size, media_bits_reader_next_bytes__jpeg))
	{
		media_verbose(data, "[deal] jpeg_parse ...");
		if (mi_jpeg_decode_load(param->jd, &reader))
		{
			media_verbose(data, "[deal] jpeg_parse ... bits-res: %zu", media_bits_reader_bits_res(&reader));
			return param;
		}
		media_warning(data, "[deal] jpeg_parse ... fail");
	}
	return NULL;
}

static d_media_runtime__emit(jpeg_parse)
{
	const struct media_frame_s *restrict sf;
	struct media_transform_param__jpeg_parse_t param;
	struct media_runtime_unit_param_t up;
	sf = (const struct media_frame_s *) step->src;
	param.jd = (struct mi_jpeg_decode_s *) step->pri;
	param.data = sf->channel_chip[3]->data;
	param.size = sf->channel_chip[3]->size;
	up.runtime = rt;
	up.context = uc;
	up.deal = (media_runtime_deal_f) media_runtime_symbol(deal, jpeg_parse);
	up.param_size = sizeof(param);
	if (media_runtime_post_unit(&up, &param, media_runtime_get_media(rt)))
		return task;
	return NULL;
}

static d_media_transform__task_append(jpeg)
{
	struct mi_jpeg_decode_s *restrict pri;
	struct mi_jpeg_decode_param_t param;
	param.fdct8x8 = ((const struct media_transform_id_jpeg_s *) tf->id)->fdct8x8;
	param.zigzag8x8 = ((const struct media_transform_id_jpeg_s *) tf->id)->zigzag8x8;
	param.f = (const uint8_t *) sf->channel_chip[0]->data;
	param.q = (const uint8_t *) sf->channel_chip[1]->data;
	param.h = (const uint8_t *) sf->channel_chip[2]->data;
	param.f_size = sf->channel_chip[0]->size;
	param.q_size = sf->channel_chip[1]->size;
	param.h_size = sf->channel_chip[2]->size;
	if ((pri = mi_jpeg_decode_alloc(&param)))
	{
		struct media_runtime_task_step_t steps[] = {
			{
				.emit = media_runtime_symbol(emit, jpeg_parse),
				.pri = pri,
				.src = sf,
				.dst = df,
			}
		};
		list = media_runtime_task_list_append(list, steps, sizeof(steps) / sizeof(*steps));
		refer_free(pri);
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
