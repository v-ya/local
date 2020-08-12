#ifndef _matrix_h_
#define _matrix_h_

#include <stdint.h>

typedef struct matrix_t {
	uint32_t m;  // 行数，列尺寸
	uint32_t n;  // 列数，行尺寸
	double v[];  // 列优先
} matrix_t;

#define matrix_size(_m, _n)     (sizeof(matrix_t) + sizeof(double) * (_m) * (_n))
#define matrix_vnumber(_m, _n)  ((uint64_t) _m * _n)

void matrix_free(matrix_t *restrict r);
matrix_t* matrix_alloc(uint32_t m, uint32_t n);
matrix_t* matrix_alloz(uint32_t m, uint32_t n);
matrix_t* matrix_zero(matrix_t *restrict r);
matrix_t* matrix_identity(matrix_t *restrict r);
matrix_t* matrix_radd(matrix_t *restrict r, const matrix_t *restrict a);
matrix_t* matrix_rsub(matrix_t *restrict r, const matrix_t *restrict a);
matrix_t* matrix_add(matrix_t *restrict r, const matrix_t *restrict a, const matrix_t *restrict b);
matrix_t* matrix_sub(matrix_t *restrict r, const matrix_t *restrict a, const matrix_t *restrict b);
matrix_t* matrix_rmul_n(matrix_t *restrict r, double x);
matrix_t* matrix_mul_n(matrix_t *restrict r, const matrix_t *restrict a, double x);
matrix_t* matrix_mul(matrix_t *restrict r, const matrix_t *restrict a, const matrix_t *restrict b);
double matrix_vec_square(const matrix_t *restrict a);
double matrix_mod(const matrix_t *restrict a);
double matrix_mod_part(const matrix_t *restrict a, uint32_t rm, uint32_t rn);
matrix_t* matrix_adjoint(matrix_t *restrict r, const matrix_t *restrict a);
matrix_t* matrix_inverse(matrix_t *restrict r, const matrix_t *restrict a);

#endif
