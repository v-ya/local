#ifndef _iphyee_glsl_array_weight_
#define _iphyee_glsl_array_weight_

#include "standard.glsl"
#include "joint.weight.glsl"

layout(buffer_reference) buffer iphyee_array_weight;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_weight {
	iphyee_joint_weight weight[];
};

#endif
