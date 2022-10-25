#include "frame.h"

static void media_frame_free_func(struct media_frame_s *restrict r)
{
	uintptr_t i;
	for (i = 0; i < r->channel; ++i)
	{
		if (r->channel_chip[i])
			refer_free(r->channel_chip[i]);
	}
	if (r->attr) refer_free(r->attr);
	if (r->id) refer_free(r->id);
}

struct media_frame_s* media_frame_alloc(const struct media_frame_id_s *restrict frame_id, uintptr_t dimension, const uintptr_t *restrict dimension_value)
{
	struct media_frame_s *restrict r;
	const struct media_cell_info_t *restrict cell;
	uintptr_t i, channel;
	if (frame_id->dimension == dimension && (r = (struct media_frame_s *) refer_alloz(
		sizeof(struct media_frame_s) + sizeof(struct media_channel_s *) * (channel = frame_id->channel))))
	{
		refer_set_free(r, (refer_free_f) media_frame_free_func);
		r->id = (const struct media_frame_id_s *) refer_save(frame_id);
		r->channel = channel;
		for (i = 0; i < channel; ++i)
		{
			if (!(cell = media_frame_id_get_cells(frame_id, i, dimension)))
				goto label_fail;
			if (!(r->channel_chip[i] = media_channel_alloc(dimension, cell, dimension_value)))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

struct media_frame_s* media_frame_set_dimension(struct media_frame_s *restrict frame, uintptr_t dimension, const uintptr_t *restrict dimension_value)
{
	const struct media_frame_id_s *restrict frame_id;
	struct media_channel_s *const *restrict channel_chip;
	const struct media_cell_info_t *restrict cell;
	uintptr_t i, channel;
	frame_id = frame->id;
	if (frame_id->dimension == dimension)
	{
		channel_chip = frame->channel_chip;
		channel = frame->channel;
		for (i = 0; i < channel; ++i)
		{
			media_channel_clear_data(channel_chip[i]);
			if (!(cell = media_frame_id_get_cells(frame_id, i, dimension)))
				frame = NULL;
			if (!media_channel_set_dimension(channel_chip[i], dimension, cell, dimension_value))
				frame = NULL;
		}
		return frame;
	}
	return NULL;
}

struct media_frame_s* media_frame_test_dimension(struct media_frame_s *restrict frame, uintptr_t dimension, const uintptr_t *restrict dimension_value)
{
	uintptr_t i, channel;
	if (frame->id->dimension == dimension)
	{
		channel = frame->channel;
		for (i = 0; i < channel; ++i)
		{
			if (!media_channel_test_dimension(frame->channel_chip[i], dimension, dimension_value))
				goto label_fail;
		}
		return frame;
	}
	label_fail:
	return NULL;
}

struct media_frame_s* media_frame_touch_data(struct media_frame_s *restrict frame)
{
	struct media_channel_s *const *restrict channel_chip;
	uintptr_t i, channel;
	channel_chip = frame->channel_chip;
	channel = frame->channel;
	for (i = 0; i < channel; ++i)
	{
		if (!media_channel_touch_data(channel_chip[i]))
			frame = NULL;
	}
	return frame;
}

void media_frame_clear_data(struct media_frame_s *restrict frame)
{
	struct media_channel_s *const *restrict channel_chip;
	uintptr_t i, channel;
	channel_chip = frame->channel_chip;
	channel = frame->channel;
	for (i = 0; i < channel; ++i)
		media_channel_clear_data(channel_chip[i]);
}
