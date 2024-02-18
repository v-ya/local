#ifndef _iphyee_glsl_array_tri3_fusion_
#define _iphyee_glsl_array_tri3_fusion_

#include "standard.glsl"
#include "tri3.fusion.glsl"

layout(buffer_reference) buffer iphyee_array_tri3_fusion;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_tri3_fusion {
	iphyee_tri3_fusion tri3[];
};

#endif
