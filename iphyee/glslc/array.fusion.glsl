#ifndef _iphyee_glsl_array_fusion_
#define _iphyee_glsl_array_fusion_

#include "standard.glsl"
#include "index.fusion.glsl"

layout(buffer_reference) buffer iphyee_array_fusion;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_fusion {
	iphyee_index_fusion fusion[];
};

#endif
