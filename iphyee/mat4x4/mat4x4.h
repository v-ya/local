#ifndef _iphyee_mat4x4_pri_h_
#define _iphyee_mat4x4_pri_h_

#include "../iphyee.mat4x4.h"
#include <math.h>

#define _mul_(row, col)  dst->m[row][col] = \
	src1->m[row][0] * src2->m[0][col] +\
	src1->m[row][1] * src2->m[1][col] +\
	src1->m[row][2] * src2->m[2][col] +\
	src1->m[row][3] * src2->m[3][col]

#define _set_(row, col, value)  dst->m[row][col] = (value)

#define _ful_(r0c0, r0c1, r0c2, r0c3, r1c0, r1c1, r1c2, r1c3, r2c0, r2c1, r2c2, r2c3, r3c0, r3c1, r3c2, r3c3)  \
	_set_(0, 0, r0c0), _set_(0, 1, r0c1), _set_(0, 2, r0c2), _set_(0, 3, r0c3), \
	_set_(1, 0, r1c0), _set_(1, 1, r1c1), _set_(1, 2, r1c2), _set_(1, 3, r1c3), \
	_set_(2, 0, r2c0), _set_(2, 1, r2c1), _set_(2, 2, r2c2), _set_(2, 3, r2c3), \
	_set_(3, 0, r3c0), _set_(3, 1, r3c1), _set_(3, 2, r3c2), _set_(3, 3, r3c3)

#endif
