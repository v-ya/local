#ifndef _iphyee_glsl_function_tri3_inside_
#define _iphyee_glsl_function_tri3_inside_

#include "tri3.inside.glsl"

void iphyee_function_tri3_inside(out iphyee_tri3_inside inside, const in vec3 tri[3])
{
	float tri_rx_1, tri_ry_1, tri_rx_2, tri_ry_2, mk;
	tri_rx_1 = tri[1].x - tri[0].x;
	tri_ry_1 = tri[1].y - tri[0].y;
	tri_rx_2 = tri[2].x - tri[0].x;
	tri_ry_2 = tri[2].y - tri[0].y;
	// < p - base_at = pos.x - tri[0].x = | tri_rx_1, tri_rx_2 | * k[0] = inv(inverse) * scale_k >
	// <               pos.y - tri[0].y = | tri_ry_1, tri_ry_2 | * k[1]                          >
	// < inverse = |  tri_ry_2, -tri_rx_2 | / (tri_rx_1 * tri_ry_2 - tri_rx_2 * tri_ry_1)        >
	// <         = | -tri_ry_1,  tri_rx_1 |                                                      >
	mk = 1.0f / (tri_rx_1 * tri_ry_2 - tri_rx_2 * tri_ry_1);
	inside.inverse_r0c0 =  tri_ry_2 * mk;
	inside.inverse_r0c1 = -tri_rx_2 * mk;
	inside.inverse_r1c0 = -tri_ry_1 * mk;
	inside.inverse_r1c1 =  tri_rx_1 * mk;
	inside.base_at_x = tri[0].x;
	inside.base_at_y = tri[0].y;
}

bool iphyee_function_tri3_inside_test(inout iphyee_tri3_inside inside, out vec2 sampling, const in float x, const in float y)
{
	float rx, ry;
	rx = x - inside.base_at_x;
	ry = y - inside.base_at_y;
	sampling[0] = inside.inverse_r0c0 * rx + inside.inverse_r0c1 * ry;
	sampling[1] = inside.inverse_r1c0 * rx + inside.inverse_r1c1 * ry;
	return (sampling[0] >= 0 && sampling[1] >= 0 && sampling[0] + sampling[1] <= 1);
}

float iphyee_function_tri3_inside_sampling(const in vec2 sampling, const in float v0, const in float v1, const in float v2)
{
	return v0 + (v1 - v0) * sampling[0] + (v2 - v0) * sampling[1];
}

vec2 iphyee_function_tri3_inside_sampling(const in vec2 sampling, const in vec2 v0, const in vec2 v1, const in vec2 v2)
{
	return v0 + (v1 - v0) * sampling[0] + (v2 - v0) * sampling[1];
}

vec3 iphyee_function_tri3_inside_sampling(const in vec2 sampling, const in vec3 v0, const in vec3 v1, const in vec3 v2)
{
	return v0 + (v1 - v0) * sampling[0] + (v2 - v0) * sampling[1];
}

#endif
