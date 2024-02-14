#ifndef _iphyee_glsl_array_vertex_
#define _iphyee_glsl_array_vertex_

#include "standard.glsl"
#include "vec3.xyz.glsl"

layout(buffer_reference) buffer iphyee_array_vertex;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_vertex {
	iphyee_vec3_xyz vertex[];
};

#endif
