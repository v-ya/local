#ifndef _iphyee_glsl_refer_model_
#define _iphyee_glsl_refer_model_

#include "standard.glsl"
#include "model.glsl"

layout(buffer_reference) buffer iphyee_refer_model;

layout(buffer_reference, std430, buffer_reference_align = 8) buffer iphyee_refer_model {
	iphyee_model model;
};

#endif
