#ifndef _iphyee_glsl_texture_pool_
#define _iphyee_glsl_texture_pool_

#include "standard.glsl"
#include "image.glsl"

layout(buffer_reference) buffer iphyee_texture_pool;

layout(buffer_reference, std430, buffer_reference_align = 8) buffer iphyee_texture_pool {
	iphyee_image texture[];
};

#endif
