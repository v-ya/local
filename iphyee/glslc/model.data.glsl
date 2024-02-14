#ifndef _iphyee_glsl_model_data_
#define _iphyee_glsl_model_data_

#include "array.vertex.glsl"
#include "array.texture.glsl"
#include "array.normal.glsl"
#include "array.fusion.glsl"

struct iphyee_model_data {
	iphyee_array_vertex data_vertex;
	iphyee_array_texture data_texture;
	iphyee_array_normal data_normal;
	iphyee_array_fusion data_fusion;
};

#endif
