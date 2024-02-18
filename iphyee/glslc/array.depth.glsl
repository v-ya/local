#ifndef _iphyee_glsl_array_depth_
#define _iphyee_glsl_array_depth_

#include "standard.glsl"

layout(buffer_reference) buffer iphyee_array_depth;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_depth {
	float depth[];
};

#endif
