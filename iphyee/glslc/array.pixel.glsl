#ifndef _iphyee_glsl_array_pixel_
#define _iphyee_glsl_array_pixel_

#include "standard.glsl"

layout(buffer_reference) buffer iphyee_array_pixel;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_pixel {
	uint pixel[];
};

#endif
