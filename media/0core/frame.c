#include "frame.h"
#include <memory.h>

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
	uintptr_t i, channel, size;
	channel = frame_id->channel;
	size = sizeof(struct media_frame_s) + sizeof(struct media_channel_s *) * (channel = frame_id->channel) + sizeof(uintptr_t) * frame_id->dimension;
	if (frame_id->dimension == dimension && (r = (struct media_frame_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) media_frame_free_func);
		r->id = (const struct media_frame_id_s *) refer_save(frame_id);
		r->dv = (uintptr_t *) (r->channel_chip + channel);
		if (dimension_value)
			memcpy(r->dv, dimension_value, sizeof(uintptr_t) * dimension);
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
		memset(frame->dv, 0, sizeof(uintptr_t) * dimension);
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
		if (frame && dimension_value)
			memcpy(frame->dv, dimension_value, sizeof(uintptr_t) * dimension);
		return frame;
	}
	return NULL;
}

const struct media_frame_s* media_frame_test_dimension(const struct media_frame_s *restrict frame, uintptr_t dimension, const uintptr_t *restrict dimension_value)
{
	if (frame->id->dimension == dimension)
	{
		if (!dimension_value || !memcmp(frame->dv, dimension_value, sizeof(uintptr_t) * dimension))
			return frame;
	}
	return NULL;
}

const struct media_frame_s* media_frame_exist_data(const struct media_frame_s *restrict frame)
{
	struct media_channel_s *const *restrict channel_chip;
	uintptr_t i, channel;
	channel_chip = frame->channel_chip;
	channel = frame->channel;
	for (i = 0; i < channel; ++i)
	{
		if (!channel_chip[i]->data && channel_chip[i]->size)
			frame = NULL;
	}
	return frame;
}

struct media_frame_s* media_frame_touch_data(struct media_frame_s *restrict frame)
{
	struct media_channel_s *const *restrict channel_chip;
	struct media_channel_s *restrict ch;
	media_frame_touch_initial_f touch_initial;
	uintptr_t i, channel;
	channel_chip = frame->channel_chip;
	channel = frame->channel;
	touch_initial = frame->id->touch_initial;
	for (i = 0; i < channel; ++i)
	{
		ch = channel_chip[i];
		if (ch->data || !ch->size)
			continue;
		if (!media_channel_touch_data(ch))
			frame = NULL;
		else if (touch_initial)
			touch_initial(ch, i);
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
