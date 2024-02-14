#ifndef _iphyee_glsl_image_
#define _iphyee_glsl_image_

#include "array.pixel.glsl"

struct iphyee_image {
	iphyee_array_pixel pixels;
	uint width;
	uint height;
};

#endif
