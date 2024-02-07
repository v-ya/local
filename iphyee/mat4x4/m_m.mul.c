#include "mat4x4.h"

void iphyee_mat4x4_mul(iphyee_mat4x4_t *restrict dst, const iphyee_mat4x4_t *restrict src1, const iphyee_mat4x4_t *restrict src2)
{
	_mul_(0, 0);
	_mul_(0, 1);
	_mul_(0, 2);
	_mul_(0, 3);
	_mul_(1, 0);
	_mul_(1, 1);
	_mul_(1, 2);
	_mul_(1, 3);
	_mul_(2, 0);
	_mul_(2, 1);
	_mul_(2, 2);
	_mul_(2, 3);
	_mul_(3, 0);
	_mul_(3, 1);
	_mul_(3, 2);
	_mul_(3, 3);
}
