#include "frame.h"
#include "codec.jpeg.h"

// channel[5]:    f, q, h, data
// dimension[4]:  ch_number, q_size, h_size, data_size

static struct media_frame_id_s* media_frame_create_image_jpeg_by_name(const char *restrict name)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(name, 4, 4)))
	{
		// f
		if (!media_frame_id_initial(r, 0, 4, (const struct media_cell_info_t [4]) {
			d_media_ci__ibase(sizeof(struct mi_jpeg_frame_ch_t)),
			d_media_ci__iplus(sizeof(struct mi_jpeg_frame_t)),
			d_media_ci__ikeep(),
			d_media_ci__ikeep(),
		})) goto label_fail;
		// q
		if (!media_frame_id_initial(r, 1, 4, (const struct media_cell_info_t [4]) {
			d_media_ci__ikeep(),
			d_media_ci__ibase(1),
			d_media_ci__ikeep(),
			d_media_ci__ikeep(),
		})) goto label_fail;
		// h
		if (!media_frame_id_initial(r, 2, 4, (const struct media_cell_info_t [4]) {
			d_media_ci__ikeep(),
			d_media_ci__ikeep(),
			d_media_ci__ibase(1),
			d_media_ci__ikeep(),
		})) goto label_fail;
		// data
		if (!media_frame_id_initial(r, 3, 4, (const struct media_cell_info_t [4]) {
			d_media_ci__ikeep(),
			d_media_ci__ikeep(),
			d_media_ci__ikeep(),
			d_media_ci__ibase(1),
		})) goto label_fail;
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

struct media_frame_id_s* media_frame_create_image_jpeg_yuv_8_411(void)
{
	return media_frame_create_image_jpeg_by_name(media_nf_jpeg_yuv_8_411);
}

struct media_frame_id_s* media_frame_create_image_jpeg_yuv_8_111(void)
{
	return media_frame_create_image_jpeg_by_name(media_nf_jpeg_yuv_8_111);
}
