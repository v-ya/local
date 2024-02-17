#ifndef _iphyee_glsl_cache_tri3_render_
#define _iphyee_glsl_cache_tri3_render_

#include "standard.glsl"
#include "tri3.render.glsl"

layout(buffer_reference) buffer iphyee_cache_tri3_render;

layout(buffer_reference, std430, buffer_reference_align = 8) buffer iphyee_cache_tri3_render {
	uint tri3_max_count;
	uint tri3_cur_count;
	iphyee_tri3_render tri3_render[];
};

#endif
