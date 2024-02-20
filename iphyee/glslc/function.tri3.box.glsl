#ifndef _iphyee_glsl_function_tri3_box_
#define _iphyee_glsl_function_tri3_box_

#include "image.glsl"
#include "tri3.box.glsl"

bool iphyee_function_tri3_box(out iphyee_tri3_box box, const in vec3 tri[3], const in iphyee_image image)
{
	box.xmin = uint(max(min(min(tri[0].x, tri[1].x), tri[2].x), 0));
	box.xmax = uint(min(max(max(tri[0].x, tri[1].x), tri[2].x), image.width - 1));
	box.ymin = uint(max(min(min(tri[0].y, tri[1].y), tri[2].y), 0));
	box.ymax = uint(min(max(max(tri[0].y, tri[1].y), tri[2].y), image.height - 1));
	return ((box.xmin <= box.xmax) && (box.ymin <= box.ymax));
}

#endif
