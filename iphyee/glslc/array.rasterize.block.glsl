#ifndef _iphyee_glsl_array_rasterize_block_
#define _iphyee_glsl_array_rasterize_block_

#include "standard.glsl"
#include "rasterize.block.glsl"

layout(buffer_reference) buffer iphyee_array_rasterize_block;

layout(buffer_reference, std430, buffer_reference_align = 8) buffer iphyee_array_rasterize_block {
	iphyee_rasterize_block block[];
};

#endif
