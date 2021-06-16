#include "inner.h"

void dsink_inner_fourier_fix(dsink_fourier_t *restrict dst, const dsink_fourier_t *restrict src)
{
	uint32_t i, n;
	if ((n = dst->rank) > src->rank)
		n = src->rank;
	dst->level += src->level;
	for (i = 0; i < n; ++i)
	{
		dst->c[i].sa += src->c[i].sa;
		dst->c[i].ca += src->c[i].ca;
	}
}
