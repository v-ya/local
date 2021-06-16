#include "inner.h"

void dsink_inner_fourier_get(dsink_fourier_t *restrict fourier, const int32_t *restrict data, uintptr_t n)
{
	uintptr_t quarter_cycle;
	uint32_t i;
	int32_t sa, ca;
	fourier->level = dsink_inner_dot_one(data, n) / fourier->cycle;
	for (i = 0; i < fourier->rank; ++i)
	{
		quarter_cycle = (fourier->cycle >> 2) / (i + 1);
		fourier->c[i].sa = sa = (int32_t) (dsink_inner_dot_sign(data, n, quarter_cycle, 0) / (intptr_t) fourier->cycle);
		fourier->c[i].ca = ca = (int32_t) (dsink_inner_dot_sign(data, n, quarter_cycle, quarter_cycle) / (intptr_t) fourier->cycle);
	}
}
