#include "transform.jpeg.h"

struct media_transform_param_t {
	const uint8_t *src;
	uint8_t *dst;
	uintptr_t src_skip;
	uintptr_t dst_skip;
	uintptr_t w;
	uintptr_t yn;
};

// bgra32 => bgr24

static d_media_runtime__deal(bgra32_bgr24, struct media_transform_param_t, refer_t)
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
			++s;
		}
		s += param->src_skip;
		d += param->dst_skip;
	}
	return param;
}

static d_media_runtime__emit(bgra32_bgr24)
{
	return media_transform_inner_image_emit__d2c1(task, step, rt, uc,
		(media_runtime_deal_f) media_runtime_symbol(deal, bgra32_bgr24), NULL);
}

static d_media_transform__task_append(bgra32_bgr24)
{
	struct media_runtime_task_step_t steps[] = {
		{
			.emit = media_runtime_symbol(emit, bgra32_bgr24),
			.pri = NULL,
			.src = sf,
			.dst = df,
		}
	};
	return media_runtime_task_list_append(list, steps, sizeof(steps) / sizeof(*steps));
}

static void media_transform_id_jpeg_free_func(struct media_transform_id_jpeg_s *restrict r)
{
	/// TODO: clear media_transform_id_jpeg_s
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
		.task_append = media_transform_symbol(task_append, bgra32_bgr24),
	};
	if ((r = (struct media_transform_id_jpeg_s *) media_transform_id_alloc(sizeof(struct media_transform_id_s), media, src_frame_id, dst_frame_id, &func)))
	{
		refer_set_free(r, (refer_free_f) media_transform_id_jpeg_free_func);
		return &r->transform;
		refer_free(r);
	}
	return NULL;
}
