#include "frame_id.h"
#include <string.h>

static void media_frame_id_free_func(struct media_frame_id_s *restrict r)
{
	if (r->name) refer_free(r->name);
	if (r->compat) refer_free(r->compat);
}

static refer_string_t media_frame_id_dump_compat(refer_string_t name)
{
	const char *restrict compat_endptr;
	if ((compat_endptr = strchr(name, ';')))
		return refer_dump_string_with_length(name, (uintptr_t) compat_endptr - (uintptr_t) name);
	return (refer_string_t) refer_save(name);
}

struct media_frame_id_s* media_frame_id_alloc(const char *restrict name, uintptr_t channel, uintptr_t dimension)
{
	struct media_frame_id_s *restrict r;
	uintptr_t n;
	if (name && (n = channel * dimension) && (r = (struct media_frame_id_s *) refer_alloz(
		sizeof(struct media_frame_id_s) + sizeof(struct media_cell_info_t) * n)))
	{
		refer_set_free(r, (refer_free_f) media_frame_id_free_func);
		if ((r->name = refer_dump_string(name)) &&
			(r->compat = media_frame_id_dump_compat(r->name)))
		{
			r->channel = channel;
			r->dimension = dimension;
			while (n)
			{
				--n;
				r->cell[n].cellsize_divisor = 1;
				r->cell[n].dimension_divisor = 1;
			}
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

struct media_frame_id_s* media_frame_id_initial(struct media_frame_id_s *restrict frame_id, uintptr_t channel_index, uintptr_t dimension, const struct media_cell_info_t *restrict params)
{
	struct media_cell_info_t *restrict ci;
	if (channel_index < frame_id->channel && dimension == frame_id->dimension)
	{
		ci = &frame_id->cell[channel_index * frame_id->dimension];
		for (channel_index = 0; channel_index < dimension; ++channel_index)
		{
			if (!media_cell_info_initial(ci + channel_index, params + channel_index))
				goto label_fail;
		}
		return frame_id;
	}
	label_fail:
	return NULL;
}

const struct media_cell_info_t* media_frame_id_get_cells(const struct media_frame_id_s *restrict frame_id, uintptr_t channel_index, uintptr_t dimension)
{
	if (channel_index < frame_id->channel && dimension == frame_id->dimension)
		return &frame_id->cell[channel_index * dimension];
	return NULL;
}
