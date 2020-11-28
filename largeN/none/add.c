#include "../largeN.h"

largeN_t* largeN_add(register largeN_t *restrict r, register const largeN_t *restrict a)
{
	register __uint128_t x;
	register uint64_t i, n;
	x = 0;
	if ((n = r->n) > a->n)
		n = a->n;
	for (i = 0; i < n; ++i)
	{
		x += r->q[i];
		x += a->q[i];
		r->q[i] = (uint64_t) x;
		x >>= 64;
	}
	if (x && n < r->n)
		return largeN_add_n(r, (uint64_t) x, n);
	return r;
}
