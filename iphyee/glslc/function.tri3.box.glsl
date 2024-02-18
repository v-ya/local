#ifndef _iphyee_glsl_function_tri3_box_
#define _iphyee_glsl_function_tri3_box_

#include "image.glsl"
#include "tri3.box.glsl"

void iphyee_function_tri3_box(out iphyee_tri3_box box, const in vec3 tri[3], const in iphyee_image image)
{
	box.xmin = uint(max(min(min(tri[0].x, tri[1].x), tri[2].x), 0));
	box.xmax = uint(min(max(max(tri[0].x, tri[1].x), tri[2].x) + 1, image.width));
	box.ymin = uint(max(min(min(tri[0].y, tri[1].y), tri[2].y), 0));
	box.ymax = uint(min(max(max(tri[0].y, tri[1].y), tri[2].y) + 1, image.height));
}

#endif
