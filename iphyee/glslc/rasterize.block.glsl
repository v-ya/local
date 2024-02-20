#ifndef _iphyee_glsl_rasterize_block_
#define _iphyee_glsl_rasterize_block_

#include "standard.glsl"

layout(buffer_reference) buffer iphyee_rasterize_block;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_rasterize_block {
	uint list_next[];
};

#endif
