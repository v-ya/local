#include "frame.h"

struct media_frame_id_s* media_frame_create_image_bgra32(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_bgra32, 1, 2)))
	{
		if (media_frame_id_initial(r, 0, 2, (const struct media_cell_info_t [2]) {
			{
				.cellsize_addend      = 4,
				.cellsize_divisor     = 1,
				.cellsize_multiplier  = 1,
				.cellsize_plus        = 0,
				.dimension_addend     = 0,
				.dimension_divisor    = 1,
				.dimension_multiplier = 1,
				.dimension_plus       = 0,
			},
			{
				.cellsize_addend      = 0,
				.cellsize_divisor     = 1,
				.cellsize_multiplier  = 1,
				.cellsize_plus        = 0,
				.dimension_addend     = 0,
				.dimension_divisor    = 1,
				.dimension_multiplier = 1,
				.dimension_plus       = 0,
			},
		})) return r;
		refer_free(r);
	}
	return r;
}

struct media_frame_id_s* media_frame_create_image_bgr24_p4(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_bgr24_p4, 1, 2)))
	{
		if (media_frame_id_initial(r, 0, 2, (const struct media_cell_info_t [2]) {
			{
				.cellsize_addend      = 3,
				.cellsize_divisor     = 1,
				.cellsize_multiplier  = 1,
				.cellsize_plus        = 0,
				.dimension_addend     = 0,
				.dimension_divisor    = 1,
				.dimension_multiplier = 1,
				.dimension_plus       = 0,
			},
			{
				.cellsize_addend      = 3,
				.cellsize_divisor     = 4,
				.cellsize_multiplier  = 4,
				.cellsize_plus        = 0,
				.dimension_addend     = 0,
				.dimension_divisor    = 1,
				.dimension_multiplier = 1,
				.dimension_plus       = 0,
			},
		})) return r;
		refer_free(r);
	}
	return r;
}
