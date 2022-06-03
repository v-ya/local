#include "channel.h"

static void media_channel_free_func(struct media_channel_s *restrict r)
{
	if (r->data_source) refer_free(r->data);
}

struct media_channel_s* media_channel_alloc(uintptr_t dimension, const struct media_cell_info_t cell[], const uintptr_t dimension_value[])
{
	struct media_channel_s *restrict r;
	uintptr_t i, j, cell_size;
	if (dimension && (r = (struct media_channel_s *) refer_alloz(sizeof(struct media_channel_s) +
		sizeof(struct media_cell_t) * dimension)))
	{
		refer_set_free(r, (refer_free_f) media_channel_free_func);
		for (i = j = cell_size = 0; i < dimension; ++i)
		{
			if (!cell[i].dimension_multiplier)
				continue;
			if (!media_cell_set((struct media_cell_t *) r->cell + j, cell + i, cell_size, dimension_value[i]))
				goto label_fail;
			cell_size = r->cell[j++].used_size;
		}
		if ((r->dimension = j) && (r->size = cell_size))
			return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

struct media_channel_s* media_channel_set_data(struct media_channel_s *restrict channel, void *restrict data, uintptr_t size, refer_t data_source)
{
	if (data && size >= channel->size)
	{
		channel->data = data;
		if (channel->data_source) refer_free(channel->data_source);
		channel->data_source = refer_save(data_source);
	}
	return NULL;
}

void media_channel_clear(struct media_channel_s *restrict channel)
{
	channel->data = NULL;
	if (channel->data_source)
	{
		refer_free(channel->data_source);
		channel->data_source = NULL;
	}
}
