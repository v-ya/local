#include "inner.h"
#include <stddef.h>

typedef struct dsink_fourier_rank2cycle_t {
	uint64_t rank;
	uint64_t cycle;
} dsink_fourier_rank2cycle_t;

uintptr_t dsink_inner_fourier_rank(uintptr_t n, uint32_t *restrict rank)
{
	static const dsink_fourier_rank2cycle_t r2c[] = {
		{ 1, 4 * 1},
		{ 2, 4 * 2},
		{ 3, 4 * 6},
		{ 4, 4 * 12},
		{ 6, 4 * 60},
		{ 7, 4 * 420},
		{ 8, 4 * 840},
		{10, 4 * 2520},
		{12, 4 * 27720},
		{15, 4 * 360360},
	};
	const dsink_fourier_rank2cycle_t *restrict p, *restrict u;
	uintptr_t i;
	u = NULL;
	for (i = 0, p = r2c; i < (sizeof(r2c) / sizeof(*r2c)); ++i)
	{
		if (n < p[i].cycle)
			break;
		u = p + i;
	}
	if (u)
	{
		*rank = u->rank;
		return u->cycle;
	}
	*rank = 0;
	return n;
}
