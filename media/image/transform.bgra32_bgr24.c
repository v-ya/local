#include "transform.h"

struct media_transform_param_t {
	const uint8_t *src;
	uint8_t *dst;
	uintptr_t src_skip;
	uintptr_t dst_skip;
	uintptr_t w;
	uintptr_t yn;
};

static d_media_runtime__deal(bgr24_bgra32, struct media_transform_param_t, refer_t)
{
	const uint8_t *restrict s;
	uint8_t *restrict d;
	uintptr_t x, y;
	s = param->src;
	d = param->dst;
	for (y = param->yn; y; --y)
	{
		for (x = param->w; x; --x)
		{
			*d++ = *s++;
			*d++ = *s++;
			*d++ = *s++;
			*d++ = 0;
		}
		s += param->src_skip;
		d += param->dst_skip;
	}
	return param;
}

static d_media_runtime__emit(bgr24_bgra32)
{
	struct media_transform_param_t param;
	struct media_runtime_unit_param_t up = {
		.runtime = rt,
		.context = uc,
		.deal = (media_runtime_deal_f) media_runtime_symbol(deal, bgr24_bgra32),
		.param_size = sizeof(param),
	};
	const struct media_channel_s *restrict src;
	struct media_channel_s *restrict dst;
	uintptr_t n, sl, dl, h, dh;
	src = ((const struct media_frame_s *) step->src)->channel_chip[0];
	dst = ((struct media_frame_s *) step->dst)->channel_chip[0];
	n = media_runtime_unit_core_number(rt);
	param.src = src->data;
	param.dst = dst->data;
	param.src_skip = (sl = src->cell[1].cell_size) - src->cell[0].cell_size * src->cell[0].cell_number;
	param.dst_skip = (dl = dst->cell[1].cell_size) - dst->cell[0].cell_size * dst->cell[0].cell_number;
	param.w = dst->cell[0].cell_number;
	param.yn = 0;
	h = dst->cell[1].cell_number;
	dh = (h + n - 1) / n;
	while (h && task)
	{
		param.yn = n = (dh < h)?dh:h;
		if (!media_runtime_post_unit(&up, &param, NULL))
			task = NULL;
		param.src += sl * n;
		param.dst += dl * n;
		h -= n;
	}
	return task;
}

static d_media_transform__post_task(bgr24_bgra32)
{
	struct media_runtime_task_step_t steps[] = {
		{
			.emit = media_runtime_symbol(emit, bgr24_bgra32),
			.pri = NULL,
			.src = sf,
			.dst = df,
		}
	};
	return media_runtime_alloc_task(tf->runtime, sizeof(steps) / sizeof(*steps), steps, done);
}

struct media_transform_id_s* media_transform_create_image__bgr24_bgra32(const struct media_s *restrict media)
{
	struct media_transform_id_func_t func = {
		.initial_judge = NULL,
		.create_pri = NULL,
		.open_codec = NULL,
		.dst_frame = NULL,
		.post_task = media_transform_symbol(post_task, bgr24_bgra32),
	};
	return media_transform_id_alloc(sizeof(struct media_transform_id_s), media, media_nf_bgr24, media_nf_bgra32, &func);
}
