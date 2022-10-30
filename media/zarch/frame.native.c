#include "frame.h"

struct media_frame_id_s* media_frame_create_zarch_native(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_native, 1, 1)))
	{
		if (media_frame_id_initial(r, 0, 1, (const struct media_cell_info_t [1]) {
			{
				.cellsize_addend      = 1,
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
