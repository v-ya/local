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
