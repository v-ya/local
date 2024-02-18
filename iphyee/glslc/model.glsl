#ifndef _iphyee_glsl_model_
#define _iphyee_glsl_model_

#include "model.inst.glsl"
#include "array.tri3.fusion.glsl"

struct iphyee_model {
	iphyee_model_inst inst;
	iphyee_array_tri3_fusion transform_fusion;
	uint fusion_offset;
	uint fusion_number;
	uint texture_index;
	uint texture_alpha;
};

#endif
