#ifndef _iphyee_glsl_array_tri3_render_
#define _iphyee_glsl_array_tri3_render_

#include "standard.glsl"
#include "tri3.render.glsl"

layout(buffer_reference) buffer iphyee_array_tri3_render;

layout(buffer_reference, std430, buffer_reference_align = 8) buffer iphyee_array_tri3_render {
	iphyee_tri3_render tri3[];
};

#endif
