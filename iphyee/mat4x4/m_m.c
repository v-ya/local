#include "mat4x4.h"
#include <memory.h>

#define _m_mul_m_(row, col)  dst->m[row][col] = \
	src1->m[row][0] * src2->m[0][col] +\
	src1->m[row][1] * src2->m[1][col] +\
	src1->m[row][2] * src2->m[2][col] +\
	src1->m[row][3] * src2->m[3][col]

#define _m_mul_v_(row)  dst->v[row] = \
	src1->m[row][0] * src2->v[0] +\
	src1->m[row][1] * src2->v[1] +\
	src1->m[row][2] * src2->v[2] +\
	src1->m[row][3] * src2->v[3]

void iphyee_mat4x4_cpy(iphyee_mat4x4_t *restrict dst, const iphyee_mat4x4_t *restrict src)
{
	memcpy(dst, src, sizeof(iphyee_mat4x4_t));
}

void iphyee_mat4x4_mul(iphyee_mat4x4_t *restrict dst, const iphyee_mat4x4_t *restrict src1, const iphyee_mat4x4_t *restrict src2)
{
	_m_mul_m_(0, 0);
	_m_mul_m_(0, 1);
	_m_mul_m_(0, 2);
	_m_mul_m_(0, 3);
	_m_mul_m_(1, 0);
	_m_mul_m_(1, 1);
	_m_mul_m_(1, 2);
	_m_mul_m_(1, 3);
	_m_mul_m_(2, 0);
	_m_mul_m_(2, 1);
	_m_mul_m_(2, 2);
	_m_mul_m_(2, 3);
	_m_mul_m_(3, 0);
	_m_mul_m_(3, 1);
	_m_mul_m_(3, 2);
	_m_mul_m_(3, 3);
}

void iphyee_mat4x4_vec4(iphyee_vec4_t *restrict dst, const iphyee_mat4x4_t *restrict src1, const iphyee_vec4_t *restrict src2)
{
	_m_mul_v_(0);
	_m_mul_v_(1);
	_m_mul_v_(2);
	_m_mul_v_(3);
}
