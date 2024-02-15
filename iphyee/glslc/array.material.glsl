#ifndef _iphyee_glsl_array_material_
#define _iphyee_glsl_array_material_

#include "standard.glsl"
#include "material.glsl"

layout(buffer_reference) buffer iphyee_array_material;

layout(buffer_reference, std430, buffer_reference_align = 4) buffer iphyee_array_material {
	iphyee_material material[];
};

#endif
