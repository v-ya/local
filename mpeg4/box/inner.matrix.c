#include "inner.matrix.h"

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
