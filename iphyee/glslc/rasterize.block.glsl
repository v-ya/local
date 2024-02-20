#ifndef _iphyee_glsl_rasterize_block_
#define _iphyee_glsl_rasterize_block_

#include "array.index.glsl"
#include "tri3.box.glsl"

struct iphyee_rasterize_block {
	iphyee_array_index tri3_index_array;
	iphyee_tri3_box block_box;
	uint index_max_count;
	uint index_cur_count;
};

#endif
