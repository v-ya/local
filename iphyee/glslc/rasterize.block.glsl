#ifndef _iphyee_rasterize_block_
#define _iphyee_rasterize_block_

#include "array.index.glsl"
#include "tri3.box.glsl"

struct iphyee_rasterize_block {
	iphyee_array_index tri3_index;
	iphyee_tri3_box tri3_box;
	uint max_count;
	uint cur_count;
};

#endif
