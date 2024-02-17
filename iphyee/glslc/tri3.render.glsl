#ifndef _iphyee_glsl_tri3_render_
#define _iphyee_glsl_tri3_render_

#include "refer.model.glsl"
#include "tri3.inside.glsl"
#include "tri3.depth.glsl"

struct iphyee_tri3_render {
	iphyee_refer_model model;
	iphyee_tri3_inside inside;
	iphyee_tri3_depth depth;
	uint fusion_index;
};

#endif
