#include "mat4x4.h"
#include <memory.h>

void iphyee_vec4_cpy(iphyee_vec4_t *restrict dst, const iphyee_vec4_t *restrict src)
{
	memcpy(dst, src, sizeof(iphyee_vec4_t));
}

void iphyee_vec4_set(iphyee_vec4_t *restrict dst, float x, float y, float z)
{
	dst->v[0] = x;
	dst->v[1] = y;
	dst->v[2] = z;
	dst->v[3] = (float) 1;
}

void iphyee_vec4_npos(iphyee_vec4_t *npos, const iphyee_vec4_t *pos4)
{
	float one, w;
	w = (one = (float) 1) / pos4->v[3];
	npos->v[0] = pos4->v[0] * w;
	npos->v[1] = pos4->v[1] * w;
	npos->v[2] = pos4->v[2] * w;
	npos->v[3] = one;
}

void iphyee_vec4_nvec(iphyee_vec4_t *nvec, const iphyee_vec4_t *vec4)
{
	float one, k;
	k = (one = (float) 1) / sqrtf(vec4->v[0] * vec4->v[0] + vec4->v[1] * vec4->v[1] + vec4->v[2] * vec4->v[2]);
	nvec->v[0] = vec4->v[0] * k;
	nvec->v[1] = vec4->v[1] * k;
	nvec->v[2] = vec4->v[2] * k;
	nvec->v[3] = one;
}

float iphyee_vec4_len3(const iphyee_vec4_t *restrict src)
{
	return sqrtf(src->v[0] * src->v[0] + src->v[1] * src->v[1] + src->v[2] * src->v[2]) / src->v[3];
}

void iphyee_vec4_quat(iphyee_vec4_t *quat, const iphyee_vec4_t *nvec_rot_axis, float ra)
{
	float ss, cc;
	ra /= 2;
	ss = sinf(ra);
	cc = cosf(ra);
	quat->v[0] = nvec_rot_axis->v[0] * ss;
	quat->v[1] = nvec_rot_axis->v[1] * ss;
	quat->v[2] = nvec_rot_axis->v[2] * ss;
	quat->v[3] = cc;
}

void iphyee_vec4_quat_mul(iphyee_vec4_t *restrict quat, const iphyee_vec4_t *restrict quat1, const iphyee_vec4_t *restrict quat2)
{
	// quat = (x1i + y1j + z1k + w1) x (x2i + y2j + z2k + w2)
	// quat = +x1x2ii +x1y2ij +x1z2ik +x1w2i
	//        +y1x2ji +y1y2jj +y1z2jk +y1w2j
	//        +z1x2ki +z1y2kj +z1z2kk +z1w2k
	//        +w1x2i  +w1y2j  +w1z2k  +w1w2
	// x = (+x1w2 +y1z2 -z1y2 +w1x2)i
	// y = (+y1w2 +z1x2 -x1z2 +w1y2)j
	// z = (+z1w2 +x1y2 -y1x2 +w1z2)k
	// w = (+w1w2 -x1x2 -y1y2 -z1z2)
	#define _m12_(_p1, _p2)  (quat1->v[_p1] * quat2->v[_p2])
	quat->v[0] = _m12_(0, 3) + _m12_(1, 2) - _m12_(2, 1) + _m12_(3, 0);
	quat->v[1] = _m12_(1, 3) + _m12_(2, 0) - _m12_(0, 2) + _m12_(3, 1);
	quat->v[2] = _m12_(2, 3) + _m12_(0, 1) - _m12_(1, 0) + _m12_(3, 2);
	quat->v[3] = _m12_(3, 3) - _m12_(0, 0) - _m12_(1, 1) - _m12_(2, 2);
	#undef _m12_
}

void iphyee_vec4_mul4(iphyee_vec4_t *dst, const iphyee_vec4_t *src1, const iphyee_vec4_t *src2)
{
	dst->v[0] = src1->v[0] * src2->v[0];
	dst->v[1] = src1->v[1] * src2->v[1];
	dst->v[2] = src1->v[2] * src2->v[2];
	dst->v[3] = src1->v[3] * src2->v[3];
}

float iphyee_vec4_dot3(const iphyee_vec4_t *restrict src1, const iphyee_vec4_t *restrict src2)
{
	return (src1->v[0] * src2->v[0] + src1->v[1] * src2->v[1] + src1->v[2] * src2->v[2]) / (src1->v[3] * src2->v[3]);
}

void iphyee_vec4_cross(iphyee_vec4_t *restrict dst, const iphyee_vec4_t *restrict src1, const iphyee_vec4_t *restrict src2)
{
	dst->v[0] = src1->v[1] * src2->v[2] - src1->v[2] * src2->v[1];
	dst->v[1] = src1->v[2] * src2->v[0] - src1->v[0] * src2->v[2];
	dst->v[2] = src1->v[0] * src2->v[1] - src1->v[1] * src2->v[0];
	dst->v[3] = src1->v[3] * src2->v[3];
}
