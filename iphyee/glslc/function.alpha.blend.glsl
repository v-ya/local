#ifndef _iphyee_glsl_function_alpha_blend_
#define _iphyee_glsl_function_alpha_blend_

#include "alpha.blend.glsl"

// e.g. far => near: c0(target), c[1] a[1], c[2] a[2], ..., c[n] a[n]
// output = ((c0(1-a[1]) + c[1]a[1])(1-a[2]) + c[2]a[2] ...)(1-a[n]) + c[n]a[n]
// now let: b[i] = 1-a[i], d[i] = c[i]a[i]
// output = c0||{1,n}b[i] + d[1]||{2,n}b[i] + ... + d[n]||{n,n}b[i]
// simplify stage:
//     color & alpha:   c0, c[1] a, c[2] a, ..., c[n] a ; a[i] => a
//     distance to c0:   0,    l[1],  l[2], ...,   l[n] ; l[i] => i * l0
// output = c0*b^n + d[1]*b^(n-1) + d[2]*b^(n-2) + ... + d[n]*b^(n-n)
// output = c0*b^n + d[1]*b^(n-l[1]/l0) + d[2]*b^(n-l[2]/l0) + ... + d[n]*b^(n-l[n]/l0)
// again let: beta = (||{1,n}(1-a[i]))^(1/n), mdepth = max(l[1], l[2], ... , l[n])/n, number = n
// then:
//    color = target_color * beta^number;
//    loop(
//        color += c[i] * a[i] * beta^(number - (target_depth - depth[i]) / mdepth);
//    );
//    output = color;
// twice loop ~ O(2n)

void iphyee_function_alpha_blend_initial(out iphyee_alpha_blend ab, const in uint target_color, const in float target_depth)
{
	vec4 c;
	c = unpackUnorm4x8(target_color);
	ab.target_b = c[0];
	ab.target_g = c[1];
	ab.target_r = c[2];
	ab.target_depth = target_depth;
	ab.mdepth = 0;
	ab.beta = 1;
	ab.number = 0;
}

void iphyee_function_alpha_blend_mark(inout iphyee_alpha_blend ab, const in uint color, const in float depth)
{
	ab.mdepth = max(ab.mdepth, ab.target_depth - depth);
	ab.beta *= (1.0f - float(color >> 24) / 255.0f);
	ab.number += 1;
}

void iphyee_function_alpha_blend_okay(inout iphyee_alpha_blend ab)
{
	ab.target_b *= ab.beta;
	ab.target_g *= ab.beta;
	ab.target_r *= ab.beta;
	ab.mdepth /= ab.number;
	ab.beta = pow(ab.beta, 1.0f / ab.number);
}

void iphyee_function_alpha_blend_push(inout iphyee_alpha_blend ab, const in uint color, const in float depth)
{
	vec4 c;
	float k;
	c = unpackUnorm4x8(color);
	k = c[3] * pow(ab.beta, ab.number - (ab.target_depth - depth) / ab.mdepth);
	ab.target_b += c[0] * k;
	ab.target_g += c[1] * k;
	ab.target_r += c[2] * k;
}

uint iphyee_function_alpha_blend_output(const in iphyee_alpha_blend ab)
{
	return packUnorm4x8(vec4(ab.target_b, ab.target_g, ab.target_r, 1.0f));
}

#endif
