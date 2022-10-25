#include "channel.h"

static void media_channel_free_func(struct media_channel_s *restrict r)
{
	if (r->data_source) refer_free(r->data);
}

struct media_channel_s* media_channel_alloc(uintptr_t dimension, const struct media_cell_info_t cell[], const uintptr_t *restrict dimension_value)
{
	struct media_channel_s *restrict r;
	if (dimension && (r = (struct media_channel_s *) refer_alloz(sizeof(struct media_channel_s) +
		sizeof(struct media_cell_t) * dimension)))
	{
		refer_set_free(r, (refer_free_f) media_channel_free_func);
		r->dimension = dimension;
		if (media_channel_set_dimension(r, dimension, cell, dimension_value))
			return r;
		refer_free(r);
	}
	return NULL;
}

struct media_channel_s* media_channel_set_dimension(struct media_channel_s *restrict channel, uintptr_t dimension, const struct media_cell_info_t cell[], const uintptr_t *restrict dimension_value)
{
	uintptr_t i, cell_size;
	if (channel->dimension == dimension)
	{
		for (i = cell_size = 0; i < dimension; ++i)
			cell_size = media_cell_set((struct media_cell_t *) channel->cell + i, cell + i, cell_size, dimension_value?dimension_value[i]:0);
		channel->size = cell_size;
		return channel;
	}
	return NULL;
}

struct media_channel_s* media_channel_test_dimension(struct media_channel_s *restrict channel, uintptr_t dimension, const uintptr_t *restrict dimension_value)
{
	uintptr_t i;
	if (channel->dimension == dimension)
	{
		for (i = 0; i < dimension; ++i)
		{
			if (channel->cell[i].cell_number != (dimension_value?dimension_value[i]:0))
				goto label_fail;
		}
		return channel;
	}
	label_fail:
	return NULL;
}

struct media_channel_s* media_channel_set_data(struct media_channel_s *restrict channel, void *restrict data, uintptr_t size, refer_t data_source)
{
	if (data && size >= channel->size)
	{
		channel->data = data;
		if (channel->data_source) refer_free(channel->data_source);
		channel->data_source = refer_save(data_source);
		return channel;
	}
	return NULL;
}

struct media_channel_s* media_channel_touch_data(struct media_channel_s *restrict channel)
{
	if (channel->data || !channel->size)
		goto label_okay;
	if (channel->data_source)
		refer_free(channel->data_source);
	if ((channel->data_source = refer_alloc(channel->size)))
	{
		channel->data = (void *) channel->data_source;
		label_okay:
		return channel;
	}
	return NULL;
}

void media_channel_clear_data(struct media_channel_s *restrict channel)
{
	channel->data = NULL;
	if (channel->data_source)
	{
		refer_free(channel->data_source);
		channel->data_source = NULL;
	}
}
