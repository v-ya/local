#include "mat4x4.h"

#define _set_(row, col, value)  dst->m[row][col] = (value)

#define _ful_(r0c0, r0c1, r0c2, r0c3, r1c0, r1c1, r1c2, r1c3, r2c0, r2c1, r2c2, r2c3, r3c0, r3c1, r3c2, r3c3)  \
	_set_(0, 0, r0c0), _set_(0, 1, r0c1), _set_(0, 2, r0c2), _set_(0, 3, r0c3), \
	_set_(1, 0, r1c0), _set_(1, 1, r1c1), _set_(1, 2, r1c2), _set_(1, 3, r1c3), \
	_set_(2, 0, r2c0), _set_(2, 1, r2c1), _set_(2, 2, r2c2), _set_(2, 3, r2c3), \
	_set_(3, 0, r3c0), _set_(3, 1, r3c1), _set_(3, 2, r3c2), _set_(3, 3, r3c3)

void iphyee_mat4x4_set_e(iphyee_mat4x4_t *restrict dst)
{
	_ful_(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

void iphyee_mat4x4_fix_kk(iphyee_mat4x4_t *restrict dst, float kx, float ky, float kz)
{
	_set_(0, 0, kx);
	_set_(1, 1, ky);
	_set_(2, 2, kz);
}

void iphyee_mat4x4_set_kk(iphyee_mat4x4_t *restrict dst, float kx, float ky, float kz)
{
	iphyee_mat4x4_set_e(dst);
	iphyee_mat4x4_fix_kk(dst, kx, ky, kz);
}

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

void iphyee_mat4x4_fix_quat(iphyee_mat4x4_t *restrict dst, const iphyee_vec4_t *restrict quat)
{
	// 1 - 2yy - 2zz, 2xy - 2zw, 2xz + 2yw
	// 2xy + 2zw, 1 - 2xx - 2zz, 2yz - 2xw
	// 2xz - 2yw, 2yz + 2xw, 1 - 2xx - 2yy
	#define x  (quat->v[0])
	#define y  (quat->v[1])
	#define z  (quat->v[2])
	#define w  (quat->v[3])
	_set_(0, 0, (float)1-2*(y*y+z*z)); _set_(0, 1, 2*(x*y-z*w)); _set_(0, 2, 2*(x*z+y*w));
	_set_(1, 0, 2*(x*y+z*w)); _set_(1, 1, (float)1-2*(x*x+z*z)); _set_(1, 2, 2*(y*z-x*w));
	_set_(2, 0, 2*(x*z-y*w)); _set_(2, 1, 2*(y*z+x*w)); _set_(2, 2, (float)1-2*(x*x+y*y));
	#undef x
	#undef y
	#undef z
	#undef w
}

void iphyee_mat4x4_set_quat(iphyee_mat4x4_t *restrict dst, const iphyee_vec4_t *restrict quat)
{
	iphyee_mat4x4_set_e(dst);
	iphyee_mat4x4_fix_quat(dst, quat);
}
