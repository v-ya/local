#ifndef _iphyee_glsl_array_texture_
#define _iphyee_glsl_array_texture_

#include "standard.glsl"
#include "vec2.uv.glsl"

layout(buffer_reference) buffer iphyee_array_texture;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_texture {
	iphyee_vec2_uv texture[];
};

#endif
