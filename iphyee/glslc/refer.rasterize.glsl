#ifndef _iphyee_glsl_refer_rasterize_
#define _iphyee_glsl_refer_rasterize_

#include "standard.glsl"
#include "rasterize.glsl"

layout(buffer_reference) buffer iphyee_refer_rasterize;

layout(buffer_reference, std430, buffer_reference_align = 8) buffer iphyee_refer_rasterize {
	iphyee_rasterize rasterize;
};

#endif
