#ifndef _iphyee_glsl_array_normal_
#define _iphyee_glsl_array_normal_

#include "standard.glsl"
#include "vec3.xyz.glsl"

layout(buffer_reference) buffer iphyee_array_normal;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_normal {
	iphyee_vec3_xyz normal[];
};

#endif
