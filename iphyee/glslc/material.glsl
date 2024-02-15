#ifndef _iphyee_glsl_material_
#define _iphyee_glsl_material_

#include "vec3.bgr.glsl"

struct iphyee_material {
	iphyee_vec3_bgr ambient;
	iphyee_vec3_bgr diffuse;
	iphyee_vec3_bgr specular;
	float shininess;
};

#endif
