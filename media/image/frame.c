#include "frame.h"

struct media_frame_id_s* media_frame_create_image_bgra32(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_bgra32, 1, 2)))
	{
		if (media_frame_id_initial(r, 0, 2, (const struct media_cell_info_t [2]) {
			d_media_ci__ibase(4),
			d_media_ci__inext(),
		})) return r;
		refer_free(r);
	}
	return NULL;
}

struct media_frame_id_s* media_frame_create_image_bgr24(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_bgr24, 1, 2)))
	{
		if (media_frame_id_initial(r, 0, 2, (const struct media_cell_info_t [2]) {
			d_media_ci__ibase(3),
			d_media_ci__inext(),
		})) return r;
		refer_free(r);
	}
	return NULL;
}

struct media_frame_id_s* media_frame_create_image_bgr24_p4(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_bgr24_p4, 1, 2)))
	{
		if (media_frame_id_initial(r, 0, 2, (const struct media_cell_info_t [2]) {
			d_media_ci__ibase(3),
			d_media_ci__inext_with_align(4),
		})) return r;
		refer_free(r);
	}
	return NULL;
}

struct media_frame_id_s* media_frame_create_image_yuv_8_411(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_yuv_8_411, 3, 2)))
	{
		static const struct media_cell_info_t ci_y[2] = {
			d_media_ci__ibase(1),
			d_media_ci__inext(),
		};
		static const struct media_cell_info_t ci_uv[2] = {
			d_media_ci__ibase_with_sink(1, 2),
			d_media_ci__inext_with_sink(2),
		};
		if (media_frame_id_initial(r, 0, 2, ci_y) &&
			media_frame_id_initial(r, 1, 2, ci_uv) &&
			media_frame_id_initial(r, 2, 2, ci_uv)
		) return r;
		refer_free(r);
	}
	return NULL;
}

struct media_frame_id_s* media_frame_create_image_yuv_8_111(void)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(media_nf_yuv_8_111, 3, 2)))
	{
		static const struct media_cell_info_t ci_yuv[2] = {
			d_media_ci__ibase(1),
			d_media_ci__inext(),
		};
		if (media_frame_id_initial(r, 0, 2, ci_yuv) &&
			media_frame_id_initial(r, 1, 2, ci_yuv) &&
			media_frame_id_initial(r, 2, 2, ci_yuv)
		) return r;
		refer_free(r);
	}
	return NULL;
}
