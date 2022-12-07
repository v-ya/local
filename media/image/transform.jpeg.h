#ifndef _media_image_transform_jpeg_h_
#define _media_image_transform_jpeg_h_

#include "transform.h"
#include "../0bits/fdct.h"

struct media_transform_id_jpeg_s {
	struct media_transform_id_s transform;
	const struct media_fdct_2d_i32_s *fdct8x8;
};

#endif
