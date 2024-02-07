#include "mat4x4.h"

void iphyee_mat4x4_set_e(iphyee_mat4x4_t *restrict dst)
{
	_ful_(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}
