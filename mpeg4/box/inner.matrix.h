#ifndef _mpeg4_inner_matrix_h_
#define _mpeg4_inner_matrix_h_

#include "box.include.h"

typedef struct inner_matrix_t {
	int32_t a; // 16.16
	int32_t b; // 16.16
	int32_t u; // 2.30
	int32_t c; // 16.16
	int32_t d; // 16.16
	int32_t v; // 2.30
	int32_t x; // 16.16
	int32_t y; // 16.16
	int32_t w; // 2.30
} __attribute__ ((packed)) inner_matrix_t;

void mpeg4$define(inner, matrix, init)(double matrix[9]);
void mpeg4$define(inner, matrix, get)(double matrix[9], const inner_matrix_t *restrict m);
void mpeg4$define(inner, matrix, set)(inner_matrix_t *restrict m, const double matrix[9]);
void mpeg4$define(inner, matrix, dump)(mlog_s *restrict mlog, const inner_matrix_t *restrict m, uint32_t level);

#endif
