#include "mat4x4.h"

void iphyee_mat4x4_fix_tr(iphyee_mat4x4_t *restrict dst, float tx, float ty, float tz)
{
	_set_(0, 3, tx);
	_set_(1, 3, ty);
	_set_(2, 3, tz);
}

void iphyee_mat4x4_set_tr(iphyee_mat4x4_t *restrict dst, float tx, float ty, float tz)
{
	iphyee_mat4x4_set_e(dst);
	iphyee_mat4x4_fix_tr(dst, tx, ty, tz);
}

void iphyee_mat4x4_fix_tx(iphyee_mat4x4_t *restrict dst, float tx)
{
	_set_(0, 3, tx);
}

void iphyee_mat4x4_set_tx(iphyee_mat4x4_t *restrict dst, float tx)
{
	iphyee_mat4x4_set_e(dst);
	iphyee_mat4x4_fix_tx(dst, tx);
}

void iphyee_mat4x4_fix_ty(iphyee_mat4x4_t *restrict dst, float ty)
{
	_set_(1, 3, ty);
}

void iphyee_mat4x4_set_ty(iphyee_mat4x4_t *restrict dst, float ty)
{
	iphyee_mat4x4_set_e(dst);
	iphyee_mat4x4_fix_ty(dst, ty);
}

void iphyee_mat4x4_fix_tz(iphyee_mat4x4_t *restrict dst, float tz)
{
	_set_(2, 3, tz);
}

void iphyee_mat4x4_set_tz(iphyee_mat4x4_t *restrict dst, float tz)
{
	iphyee_mat4x4_set_e(dst);
	iphyee_mat4x4_fix_tz(dst, tz);
}
