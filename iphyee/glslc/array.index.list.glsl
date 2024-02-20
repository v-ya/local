#ifndef _iphyee_glsl_array_index_list_
#define _iphyee_glsl_array_index_list_

#include "standard.glsl"
#include "index.list.glsl"

layout(buffer_reference) buffer iphyee_array_index_list;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_index_list {
	iphyee_index_list list[];
};

#endif
