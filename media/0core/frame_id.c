#include "frame_id.h"

static void media_frame_id_free_func(struct media_frame_id_s *restrict r)
{
	if (r->name) refer_free(r->name);
}

struct media_frame_id_s* media_frame_id_alloc(const char *restrict name, uintptr_t channel, uintptr_t dimension)
{
	struct media_frame_id_s *restrict r;
	uintptr_t n;
	if (name && (n = channel * dimension) && (r = (struct media_frame_id_s *) refer_alloz(
		sizeof(struct media_frame_id_s) + sizeof(struct media_cell_info_t) * n)))
	{
		refer_set_free(r, (refer_free_f) media_frame_id_free_func);
		if ((r->name = refer_dump_string(name)))
		{
			r->channel = channel;
			r->dimension = dimension;
			while (n)
			{
				--n;
				r->cell[n].cell_align = 1;
				r->cell[n].dimension_divisor = 1;
				r->cell[n].size_align = 1;
			}
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

struct media_frame_id_s* media_frame_id_initial(struct media_frame_id_s *restrict frame_id, uintptr_t channel, uintptr_t dimension, uintptr_t cell_addend, uintptr_t cell_align, uintptr_t dimension_addend, uintptr_t dimension_multiplier, uintptr_t dimension_divisor, uintptr_t size_align)
{
	if (channel < frame_id->channel && dimension < frame_id->dimension &&
		media_cell_info_initial(&frame_id->cell[channel * frame_id->dimension + dimension],
			cell_addend, cell_align, dimension_addend, dimension_multiplier, dimension_divisor, size_align))
		return frame_id;
	return NULL;
}

struct media_channel_s* media_frame_id_create_channel(const struct media_frame_id_s *restrict frame_id, uintptr_t channel, uintptr_t dimension, const uintptr_t dimension_value[])
{
	if (channel < frame_id->channel && dimension == frame_id->dimension)
		return media_channel_alloc(dimension, &frame_id->cell[channel * dimension], dimension_value);
	return NULL;
}
