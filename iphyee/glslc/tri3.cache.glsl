#ifndef _iphyee_glsl_tri3_cache_
#define _iphyee_glsl_tri3_cache_

#include "array.tri3.render.glsl"

struct iphyee_tri3_cache {
	iphyee_array_tri3_render tri3_array;
	uint tri3_max_count;
	uint tri3_cur_count;
};

#endif
