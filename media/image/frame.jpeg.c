#include "frame.h"
#include "codec.jpeg.h"

// channel[5]:    frame, ch, q, h, data
// dimension[4]:  ch_number, q_size, h_size, data_size

#define d_cell_info__cell(_size)  \
	.cellsize_addend      = _size,\
	.cellsize_divisor     = 1,\
	.cellsize_multiplier  = 1,\
	.cellsize_plus        = 0
#define d_cell_info__dv()  \
	.dimension_addend     = 0,\
	.dimension_divisor    = 1,\
	.dimension_multiplier = 1,\
	.dimension_plus       = 0
#define d_cell_info__d1()  \
	.dimension_addend     = 0,\
	.dimension_divisor    = 1,\
	.dimension_multiplier = 0,\
	.dimension_plus       = 1

#define d_cell_info_keep()       { d_cell_info__cell(0), d_cell_info__d1() }
#define d_cell_info_must(_size)  { d_cell_info__cell(_size), d_cell_info__d1() }
#define d_cell_info_base(_size)  { d_cell_info__cell(_size), d_cell_info__dv() }

static struct media_frame_id_s* media_frame_create_image_jpeg_by_name(const char *restrict name)
{
	struct media_frame_id_s *restrict r;
	if ((r = media_frame_id_alloc(name, 5, 4)))
	{
		// frame
		if (!media_frame_id_initial(r, 0, 4, (const struct media_cell_info_t [4]) {
			d_cell_info_must(sizeof(struct mi_jpeg_frame_t)),
			d_cell_info_keep(),
			d_cell_info_keep(),
			d_cell_info_keep(),
		})) goto label_fail;
		// ch
		if (!media_frame_id_initial(r, 1, 4, (const struct media_cell_info_t [4]) {
			d_cell_info_base(sizeof(struct mi_jpeg_frame_ch_t)),
			d_cell_info_keep(),
			d_cell_info_keep(),
			d_cell_info_keep(),
		})) goto label_fail;
		// q
		if (!media_frame_id_initial(r, 2, 4, (const struct media_cell_info_t [4]) {
			d_cell_info_keep(),
			d_cell_info_base(1),
			d_cell_info_keep(),
			d_cell_info_keep(),
		})) goto label_fail;
		// h
		if (!media_frame_id_initial(r, 3, 4, (const struct media_cell_info_t [4]) {
			d_cell_info_keep(),
			d_cell_info_keep(),
			d_cell_info_base(1),
			d_cell_info_keep(),
		})) goto label_fail;
		// data
		if (!media_frame_id_initial(r, 4, 4, (const struct media_cell_info_t [4]) {
			d_cell_info_keep(),
			d_cell_info_keep(),
			d_cell_info_keep(),
			d_cell_info_base(1),
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
