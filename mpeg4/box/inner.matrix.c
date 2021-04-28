#include "inner.matrix.h"
#include "inner.data.h"

void mpeg4$define(inner, matrix, init)(double matrix[9])
{
	matrix[0] = 1;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;
	matrix[4] = 1;
	matrix[5] = 0;
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = 1;
}

void mpeg4$define(inner, matrix, get)(double matrix[9], const inner_matrix_t *restrict m)
{
	matrix[0] = mpeg4_fix_point(mpeg4_n32(m->a), 16, 16);
	matrix[1] = mpeg4_fix_point(mpeg4_n32(m->b), 16, 16);
	matrix[2] = mpeg4_fix_point(mpeg4_n32(m->u), 2, 30);
	matrix[3] = mpeg4_fix_point(mpeg4_n32(m->c), 16, 16);
	matrix[4] = mpeg4_fix_point(mpeg4_n32(m->d), 16, 16);
	matrix[5] = mpeg4_fix_point(mpeg4_n32(m->v), 2, 30);
	matrix[6] = mpeg4_fix_point(mpeg4_n32(m->x), 16, 16);
	matrix[7] = mpeg4_fix_point(mpeg4_n32(m->y), 16, 16);
	matrix[8] = mpeg4_fix_point(mpeg4_n32(m->w), 2, 30);
}

void mpeg4$define(inner, matrix, set)(inner_matrix_t *restrict m, const double matrix[9])
{
	m->a = mpeg4_n32(mpeg4_fix_point_gen(matrix[0], int32_t, 16, 16));
	m->b = mpeg4_n32(mpeg4_fix_point_gen(matrix[1], int32_t, 16, 16));
	m->u = mpeg4_n32(mpeg4_fix_point_gen(matrix[2], int32_t, 2, 30));
	m->c = mpeg4_n32(mpeg4_fix_point_gen(matrix[3], int32_t, 16, 16));
	m->d = mpeg4_n32(mpeg4_fix_point_gen(matrix[4], int32_t, 16, 16));
	m->v = mpeg4_n32(mpeg4_fix_point_gen(matrix[5], int32_t, 2, 30));
	m->x = mpeg4_n32(mpeg4_fix_point_gen(matrix[6], int32_t, 16, 16));
	m->y = mpeg4_n32(mpeg4_fix_point_gen(matrix[7], int32_t, 16, 16));
	m->w = mpeg4_n32(mpeg4_fix_point_gen(matrix[8], int32_t, 2, 30));
}

void mpeg4$define(inner, matrix, dump)(mlog_s *restrict mlog, const inner_matrix_t *restrict m, uint32_t level)
{
	#define matrix_format  "%12g, %12g, %12g\n"
	mlog_level_dump(matrix_format,
		mpeg4_fix_point(mpeg4_n32(m->a), 16, 16),
		mpeg4_fix_point(mpeg4_n32(m->b), 16, 16),
		mpeg4_fix_point(mpeg4_n32(m->u), 2, 30));
	mlog_level_dump(matrix_format,
		mpeg4_fix_point(mpeg4_n32(m->c), 16, 16),
		mpeg4_fix_point(mpeg4_n32(m->d), 16, 16),
		mpeg4_fix_point(mpeg4_n32(m->v), 2, 30));
	mlog_level_dump(matrix_format,
		mpeg4_fix_point(mpeg4_n32(m->x), 16, 16),
		mpeg4_fix_point(mpeg4_n32(m->y), 16, 16),
		mpeg4_fix_point(mpeg4_n32(m->w), 2, 30));
	#undef matrix_format
}
