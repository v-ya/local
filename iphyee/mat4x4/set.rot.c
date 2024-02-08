#include "mat4x4.h"

void iphyee_mat4x4_fix_rx(iphyee_mat4x4_t *restrict dst, float rx)
{
	float ss, cc;
	ss = sinf(rx);
	cc = cosf(rx);
	_set_(1, 1, cc); _set_(1, 2, -ss);
	_set_(2, 1, ss); _set_(2, 2, cc);
}

void iphyee_mat4x4_set_rx(iphyee_mat4x4_t *restrict dst, float rx)
{
	iphyee_mat4x4_set_e(dst);
	iphyee_mat4x4_fix_rx(dst, rx);
}

void iphyee_mat4x4_fix_ry(iphyee_mat4x4_t *restrict dst, float ry)
{
	float ss, cc;
	ss = sinf(ry);
	cc = cosf(ry);
	_set_(0, 0, cc); _set_(0, 2, ss);
	_set_(2, 0, -ss); _set_(2, 2, cc);
}

void iphyee_mat4x4_set_ry(iphyee_mat4x4_t *restrict dst, float ry)
{
	iphyee_mat4x4_set_e(dst);
	iphyee_mat4x4_fix_ry(dst, ry);
}

void iphyee_mat4x4_fix_rz(iphyee_mat4x4_t *restrict dst, float rz)
{
	float ss, cc;
	ss = sinf(rz);
	cc = cosf(rz);
	_set_(0, 0, cc); _set_(0, 1, -ss);
	_set_(1, 0, ss); _set_(1, 1, cc);
}

void iphyee_mat4x4_set_rz(iphyee_mat4x4_t *restrict dst, float rz)
{
	iphyee_mat4x4_set_e(dst);
	iphyee_mat4x4_fix_rz(dst, rz);
}
