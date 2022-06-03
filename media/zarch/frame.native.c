#include "frame.h"

struct media_frame_id_s* media_frame_create_zarch_native(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_name_frame_native, 1, 1)))
	{
		if (media_frame_id_initial(r, 0, 0, 1, 1, 0, 1, 1, 1))
			return r;
		refer_free(r);
	}
	return r;
}
