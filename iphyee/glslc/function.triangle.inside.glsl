#ifndef _iphyee_glsl_function_triangle_inside_
#define _iphyee_glsl_function_triangle_inside_

struct iphyee_triangle_inside {
	mat2 inverse;
	vec2 b;
	vec2 k;
};

void iphyee_function_triangle_inside(out iphyee_triangle_inside inside, const in vec3 tri[3])
{
	float tri_rx_1, tri_ry_1, tri_rx_2, tri_ry_2;
	tri_rx_1 = tri[1].x - tri[0].x;
	tri_ry_1 = tri[1].y - tri[0].y;
	tri_rx_2 = tri[2].x - tri[0].x;
	tri_ry_2 = tri[2].y - tri[0].y;
	// < p - b = pos.x - tri[0].x = | tri_rx_1, tri_rx_2 | * k[0] = inv(inverse) * k      >
	// <         pos.y - tri[0].y = | tri_ry_1, tri_ry_2 | * k[1]                         >
	// < inverse = |  tri_ry_2, -tri_ry_1 | / (tri_rx_1 * tri_ry_2 - tri_rx_2 * tri_ry_1) >
	// <         = | -tri_rx_2,  tri_rx_1 |                                               >
	inside.inverse = mat2(
				tri_ry_2, -tri_ry_1,
				-tri_rx_2, tri_rx_1
			) / (tri_rx_1 * tri_ry_2 - tri_rx_2 * tri_ry_1);
	inside.b = vec2(tri[0].x, tri[0].y);
}

bool iphyee_function_triangle_inside_test(inout iphyee_triangle_inside inside, const float x, const float y)
{
	inside.k = inside.inverse * (vec2(x, y) - inside.b);
	return (inside.k[0] >= 0 && inside.k[1] >= 0 && inside.k[0] + inside.k[1] <= 1);
}

float iphyee_function_triangle_inside_sampling(const in iphyee_triangle_inside inside, const float v0, const float v1, const float v2)
{
	return v0 + (v1 - v0) * inside.k[0] + (v2 - v0) * inside.k[1];
}

vec2 iphyee_function_triangle_inside_sampling(const in iphyee_triangle_inside inside, const vec2 v0, const vec2 v1, const vec2 v2)
{
	return v0 + (v1 - v0) * inside.k[0] + (v2 - v0) * inside.k[1];
}

vec3 iphyee_function_triangle_inside_sampling(const in iphyee_triangle_inside inside, const vec3 v0, const vec3 v1, const vec3 v2)
{
	return v0 + (v1 - v0) * inside.k[0] + (v2 - v0) * inside.k[1];
}

#endif
