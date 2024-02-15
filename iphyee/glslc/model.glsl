#ifndef _iphyee_glsl_model_
#define _iphyee_glsl_model_

#include "model.data.glsl"
#include "model.count.glsl"

struct iphyee_model {
	mat4 transform;
	iphyee_model_data data;
	iphyee_model_count count;
	uint texture_id;
};

#endif
