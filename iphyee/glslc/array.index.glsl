#ifndef _iphyee_glsl_array_index_
#define _iphyee_glsl_array_index_

#include "standard.glsl"

layout(buffer_reference) buffer iphyee_array_index;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_index {
	uint index[];
};

#endif
