#include "../largeN.h"

largeN_t* largeN_add_n(register largeN_t *restrict r, uint64_t v, register uint64_t i)
{
	register __uint128_t x;
	register uint64_t n;
	x = v;
	n = r->n;
	while (x && i < n)
	{
		x += r->q[i];
		r->q[i] = (uint64_t) x;
		x >>= 64;
		++i;
	}
	return r;
}
