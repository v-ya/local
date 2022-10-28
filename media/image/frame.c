#include "frame.h"

struct media_frame_id_s* media_frame_create_image_bgra32(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_bgra32, 1, 2)))
	{
		if (
			media_frame_id_initial(r, 0, 0, 4, 1, 0, 1, 1, 1) &&
			media_frame_id_initial(r, 0, 1, 0, 1, 0, 1, 1, 1)
		) return r;
		refer_free(r);
	}
	return r;
}

struct media_frame_id_s* media_frame_create_image_bgr24_p4(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_bgr24_p4, 1, 2)))
	{
		if (
			media_frame_id_initial(r, 0, 0, 3, 1, 0, 1, 1, 4) &&
			media_frame_id_initial(r, 0, 1, 0, 1, 0, 1, 1, 1)
		) return r;
		refer_free(r);
	}
	return r;
}
