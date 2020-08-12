#include "matrix.h"
#include <stdlib.h>
#include <memory.h>
#include <alloca.h>

void matrix_free(register matrix_t *restrict r)
{
	free(r);
}

matrix_t* matrix_alloc(register uint32_t m, register uint32_t n)
{
	register matrix_t *restrict r;
	r = NULL;
	if (m && n)
	{
		r = (matrix_t *) malloc(matrix_size(m, n));
		if (r)
		{
			r->m = m;
			r->n = n;
		}
	}
	return r;
}

matrix_t* matrix_alloz(register uint32_t m, register uint32_t n)
{
	register matrix_t *restrict r;
	r = NULL;
	if (m && n)
	{
		r = (matrix_t *) calloc(1, matrix_size(m, n));
		if (r)
		{
			r->m = m;
			r->n = n;
		}
	}
	return r;
}

matrix_t* matrix_zero(register matrix_t *restrict r)
{
	memset(r->v, 0, (size_t) matrix_vnumber(r->m, r->n) * sizeof(double));
	return r;
}

matrix_t* matrix_identity(register matrix_t *restrict r)
{
	register size_t n, i, p;
	memset(r->v, 0, (size_t) (n = matrix_vnumber((p = r->m), r->n)) * sizeof(double));
	i = 0;
	++p;
	do {
		r->v[i] = 1;
		i += p;
	} while (i < n);
	return r;
}

matrix_t* matrix_radd(register matrix_t *restrict r, register const matrix_t *restrict a)
{
	if (r->m == a->m && r->n == a->n)
	{
		register uint64_t n;
		n = matrix_vnumber(r->m, r->n);
		do {
			--n;
			r->v[n] += a->v[n];
		} while (n);
		return r;
	}
	return NULL;
}

matrix_t* matrix_rsub(register matrix_t *restrict r, register const matrix_t *restrict a)
{
	if (r->m == a->m && r->n == a->n)
	{
		register uint64_t n;
		n = matrix_vnumber(r->m, r->n);
		do {
			--n;
			r->v[n] -= a->v[n];
		} while (n);
		return r;
	}
	return NULL;
}

matrix_t* matrix_add(register matrix_t *restrict r, register const matrix_t *restrict a, register const matrix_t *restrict b)
{
	if (r->m == a->m && r->m == b->m && r->n == a->n && r->n == b->n)
	{
		register uint64_t n;
		n = matrix_vnumber(r->m, r->n);
		do {
			--n;
			r->v[n] = a->v[n] + b->v[n];
		} while (n);
		return r;
	}
	return NULL;
}

matrix_t* matrix_sub(register matrix_t *restrict r, register const matrix_t *restrict a, register const matrix_t *restrict b)
{
	if (r->m == a->m && r->m == b->m && r->n == a->n && r->n == b->n)
	{
		register uint64_t n;
		n = matrix_vnumber(r->m, r->n);
		do {
			--n;
			r->v[n] = a->v[n] - b->v[n];
		} while (n);
		return r;
	}
	return NULL;
}

matrix_t* matrix_rmul_n(register matrix_t *restrict r, register double x)
{
	register uint64_t n;
	n = matrix_vnumber(r->m, r->n);
	do {
		--n;
		r->v[n] *= x;
	} while (n);
	return r;
}

matrix_t* matrix_mul_n(register matrix_t *restrict r, register const matrix_t *restrict a, register double x)
{
	if (r->m == a->m && r->n == a->n)
	{
		register uint64_t n;
		n = matrix_vnumber(r->m, r->n);
		do {
			--n;
			r->v[n] = a->v[n] * x;
		} while (n);
		return r;
	}
	return NULL;
}

matrix_t* matrix_mul(matrix_t *restrict r, const matrix_t *restrict a, const matrix_t *restrict b)
{
	if (r->m == a->m && r->n == b->n && a->n == b->m)
	{
		register double *restrict rr;
		register const double *restrict aa, *restrict bb;
		register double vv;
		uint32_t i, j, k, m, u;
		m = r->m;
		i = r->n;
		u = a->n;
		rr = r->v + ((uint64_t) m * i);
		bb = b->v + ((uint64_t) u * i);
		do {
			--i;
			j = m;
			aa = a->v + m;
			bb -= u;
			do {
				--j;
				vv = 0;
				k = u;
				--aa;
				do {
					--k;
					vv += aa[(uint64_t) k * u] * bb[k];
				} while (k);
				*--rr = vv;
			} while (j);
		} while (i);
		return r;
	}
	return NULL;
}

double matrix_vec_square(register const matrix_t *restrict a)
{
	register double m;
	m = 0;
	if (a->m == 1 || a->n == 1)
	{
		register uint32_t n;
		register double v;
		n = a->m * a->n;
		do {
			v = a->v[--n];
			m += v * v;
		} while (n);
	}
	return m;
}

// 之下需要重写
struct matrix_pri_mod_t {
	uint32_t i;
	uint32_t u;
	double x;
	double s;
};

static double matrix_mod_part_loop(register const double *restrict v, register uint32_t N, register struct matrix_pri_mod_t *restrict p, register uint8_t *restrict mask, register uint32_t n, uint32_t rn)
{
	register double x;
	register uint32_t i;
	uint32_t nn;
	nn = n;
	p[n].s = 1;
	label_loop:
	if (p->i < n)
	{
		i = p->u;
		while (mask[i]) ++i;
		mask[i] = 1;
		p->u = i;
		if (++(p->i) & 1) x = v[(uint64_t) rn % N * N + i];
		else x = -v[(uint64_t) rn % N * N + i];
		--n;
		++rn;
		(++p)->x = x;
		if (n) goto label_loop;
	}
	p->i = p->u = 0;
	x = (p->x *= p->s);
	p->s = !n;
	if (++n > nn) goto label_return;
	--rn;
	(--p)->s += x;
	mask[i = p->u] = 0;
	p->u = ++i;
	goto label_loop;
	label_return:
	return x;
}

double matrix_mod(register const matrix_t *restrict a)
{
	register uint32_t n;
	if ((n = a->m) == a->n)
	{
		register uint8_t *restrict mask;
		register struct matrix_pri_mod_t *restrict p;
		++n;
		p = (struct matrix_pri_mod_t *) alloca(sizeof(struct matrix_pri_mod_t) * n);
		mask = (uint8_t *) alloca(n);
		memset(p, 0, sizeof(struct matrix_pri_mod_t) * n);
		memset(mask, 0, n);
		--n;
		p->x = 1;
		return matrix_mod_part_loop(a->v, n, p, mask, n, 0);
	}
	return 0;
}

double matrix_mod_part(register const matrix_t *restrict a, uint32_t rm, uint32_t rn)
{
	register uint32_t n;
	if ((n = a->m) == a->n && rm < n && rn < n)
	{
		register uint8_t *restrict mask;
		register struct matrix_pri_mod_t *restrict p;
		p = (struct matrix_pri_mod_t *) alloca(sizeof(struct matrix_pri_mod_t) * n);
		mask = (uint8_t *) alloca(n);
		memset(p, 0, sizeof(struct matrix_pri_mod_t) * n);
		memset(mask, 0, n);
		mask[rm] = 1;
		p->x = ((rm ^ (rn & ~n)) & 1)?-1:1;
		return matrix_mod_part_loop(a->v, n, p, mask, n - 1, rn + 1);
	}
	return 0;
}

matrix_t* matrix_adjoint(matrix_t *restrict r, const matrix_t *restrict a)
{
	uint32_t n;
	if ((n = r->m) == r->n && a->m == n && a->n == n)
	{
		uint8_t *restrict mask;
		struct matrix_pri_mod_t *restrict p;
		register double *restrict v;
		register uint32_t i, j;
		p = (struct matrix_pri_mod_t *) alloca(sizeof(struct matrix_pri_mod_t) * n);
		mask = (uint8_t *) alloca(n);
		memset(p, 0, sizeof(struct matrix_pri_mod_t) * n);
		memset(mask, 0, n);
		v = r->v + (uint64_t) n * n;
		i = n;
		do {
			--i;
			j = n;
			do {
				mask[i] = 1;
				p->x = ((i ^ (j | n)) & 1)?1:-1;
				*--v = matrix_mod_part_loop(a->v, n, p, mask, n - 1, j);
				mask[i] = 0;
			} while (--j);
		} while (i);
		return r;
	}
	return NULL;
}

matrix_t* matrix_inverse(register matrix_t *restrict r, register const matrix_t *restrict a)
{
	if (matrix_adjoint(r, a))
	{
		register double m;
		register const double *restrict adj, *restrict src;
		register uint32_t n, i;
		m = 0;
		n = r->m;
		adj = r->v;
		src = a->v;
		i = 0;
		do {
			m += *adj * *src;
			++adj;
			src += n;
		} while (++i < n);
		if (m) return matrix_rmul_n(r, 1.0 / m);
	}
	return NULL;
}
