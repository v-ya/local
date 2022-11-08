#include "transform.h"

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

static d_media_transform__post_task(bgra32_bgr24)
{
	struct media_runtime_task_step_t steps[] = {
		{
			.emit = media_runtime_symbol(emit, bgra32_bgr24),
			.pri = NULL,
			.src = sf,
			.dst = df,
		}
	};
	return media_runtime_alloc_task(tf->runtime, sizeof(steps) / sizeof(*steps), steps, done);
}

struct media_transform_id_s* media_transform_create_image__bgra32_bgr24(const struct media_s *restrict media)
{
	struct media_transform_id_func_t func = {
		.initial_judge = NULL,
		.create_pri = NULL,
		.open_codec = NULL,
		.dst_frame = NULL,
		.post_task = media_transform_symbol(post_task, bgra32_bgr24),
	};
	return media_transform_id_alloc(sizeof(struct media_transform_id_s), media, media_nf_bgra32, media_nf_bgr24, &func);
}

// bgr24 => bgra32

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
	return media_transform_inner_image_emit__d2c1(task, step, rt, uc,
		(media_runtime_deal_f) media_runtime_symbol(deal, bgr24_bgra32), NULL);
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
