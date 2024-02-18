#ifndef _iphyee_glsl_array_effect_
#define _iphyee_glsl_array_effect_

#include "standard.glsl"
#include "joint.effect.glsl"

layout(buffer_reference) buffer iphyee_array_effect;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_effect {
	iphyee_joint_effect effect[];
};

#endif
