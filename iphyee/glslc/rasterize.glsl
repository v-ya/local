#ifndef _iphyee_rasterize_
#define _iphyee_rasterize_

#include "image.glsl"
#include "array.depth.glsl"
#include "texture.pool.glsl"
#include "cache.tri3.render.glsl"
#include "array.rasterize.block.glsl"

struct iphyee_rasterize_block {
	iphyee_image image;
	iphyee_array_depth depth;
	iphyee_texture_pool tpool;
	iphyee_cache_tri3_render tri3_normal;
	iphyee_cache_tri3_render tri3_alpha;
	iphyee_array_rasterize_block block_normal;
	iphyee_array_rasterize_block block_alpha;
	uint block_width;
	uint block_height;
	uint block_h_count;
	uint block_v_count;
};

#endif
