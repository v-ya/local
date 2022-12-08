#include "transform.h"

void* media_transform_inner_image_cinfo(const struct media_channel_s *restrict channel, uintptr_t *restrict width, uintptr_t *restrict height, uintptr_t *restrict linesize, uintptr_t *restrict lineskip)
{
	if (channel->dimension == 2)
	{
		if (width) *width = channel->cell[0].cell_number;
		if (height) *height = channel->cell[1].cell_number;
		if (linesize) *linesize = channel->cell[1].cell_size;
		if (lineskip) *lineskip = channel->cell[1].cell_size - (channel->cell[0].cell_size * channel->cell[0].cell_number);
		return channel->data;
	}
	return NULL;
}

// d2c1

struct media_runtime_task_s* media_transform_inner_image_emit__d2c1(struct media_runtime_task_s *restrict task, const struct media_frame_s *restrict sf, struct media_frame_s *restrict df, struct media_runtime_s *restrict rt, struct media_runtime_unit_context_s *restrict uc, media_runtime_deal_f deal, refer_t data)
{
	struct media_transform_param__d2c1_t param;
	uintptr_t n, sl, dl, h, dh;
	if (media_frame_set_dimension(df, sf->id->dimension, sf->dv) && media_frame_touch_data(df) &&
		(param.src = media_transform_inner_image_cinfo(sf->channel_chip[0], &param.w, &h, &sl, &param.src_skip)) &&
		(param.dst = media_transform_inner_image_cinfo(df->channel_chip[0], NULL, NULL, &dl, &param.dst_skip)))
	{
		struct media_runtime_unit_param_t up = {
			.runtime = rt,
			.context = uc,
			.deal = deal,
			.param_size = sizeof(param),
		};
		n = media_runtime_unit_core_number(rt);
		dh = (h + n - 1) / n;
		while (h && task)
		{
			param.yn = n = (dh < h)?dh:h;
			if (!media_runtime_post_unit(&up, &param, data))
				task = NULL;
			param.src += sl * n;
			param.dst += dl * n;
			h -= n;
		}
		return task;
	}
	return NULL;
}
