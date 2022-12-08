#include "transform.h"

// bgra32 => bgr24

static d_media_runtime__deal(bgra32_bgr24, struct media_transform_param__d2c1_t, refer_t)
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
	return media_transform_inner_image_emit__d2c1(task,
		(const struct media_frame_s *) pri_data[0],
		(struct media_frame_s *) pri_data[1], rt, uc,
		(media_runtime_deal_f) media_runtime_symbol(deal, bgra32_bgr24), NULL);
}

static d_media_transform__task_append(bgra32_bgr24)
{
	struct media_runtime_task_step_t steps[] = {
		{
			.emit = media_runtime_symbol(emit, bgra32_bgr24),
			.pri_number = 2,
			.pri_data = (const refer_t []) {sf, df}
		}
	};
	return media_runtime_task_list_append(list, steps, sizeof(steps) / sizeof(*steps));
}

struct media_transform_id_s* media_transform_create_image__bgra32_bgr24(const struct media_s *restrict media)
{
	struct media_transform_id_func_t func = {
		.initial_judge = NULL,
		.create_pri = NULL,
		.open_codec = NULL,
		.dst_frame = NULL,
		.task_append = media_transform_symbol(task_append, bgra32_bgr24),
	};
	return media_transform_id_alloc(sizeof(struct media_transform_id_s), media, media_nf_bgra32, media_nf_bgr24, &func);
}

// bgr24 => bgra32

static d_media_runtime__deal(bgr24_bgra32, struct media_transform_param__d2c1_t, refer_t)
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
	return media_transform_inner_image_emit__d2c1(task,
		(const struct media_frame_s *) pri_data[0],
		(struct media_frame_s *) pri_data[1], rt, uc,
		(media_runtime_deal_f) media_runtime_symbol(deal, bgr24_bgra32), NULL);
}

static d_media_transform__task_append(bgr24_bgra32)
{
	struct media_runtime_task_step_t steps[] = {
		{
			.emit = media_runtime_symbol(emit, bgr24_bgra32),
			.pri_number = 2,
			.pri_data = (const refer_t []) {sf, df}
		}
	};
	return media_runtime_task_list_append(list, steps, sizeof(steps) / sizeof(*steps));
}

struct media_transform_id_s* media_transform_create_image__bgr24_bgra32(const struct media_s *restrict media)
{
	struct media_transform_id_func_t func = {
		.initial_judge = NULL,
		.create_pri = NULL,
		.open_codec = NULL,
		.dst_frame = NULL,
		.task_append = media_transform_symbol(task_append, bgr24_bgra32),
	};
	return media_transform_id_alloc(sizeof(struct media_transform_id_s), media, media_nf_bgr24, media_nf_bgra32, &func);
}
