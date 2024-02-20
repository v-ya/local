#ifndef _iphyee_glsl_rasterize_
#define _iphyee_glsl_rasterize_

#include "image.glsl"
#include "array.depth.glsl"
#include "texture.pool.glsl"
#include "rasterize.block.glsl"
#include "array.tri3.render.glsl"
#include "array.index.list.glsl"

struct iphyee_rasterize {
	iphyee_image image;
	iphyee_array_depth depth;
	iphyee_texture_pool tpool;
	iphyee_array_tri3_render tri3;
	iphyee_array_index_list list;
	iphyee_rasterize_block block_normal;
	iphyee_rasterize_block block_alpha;
	uint tri3_max_count;
	uint tri3_cur_count;
	uint list_max_count;
	uint list_cur_count;
	uint block_width;
	uint block_height;
	uint block_h_count;
	uint block_v_count;
};

#endif
