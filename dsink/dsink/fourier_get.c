#include "inner.h"

void dsink_inner_fourier_get(dsink_fourier_t *restrict fourier, const int32_t *restrict data, uintptr_t n)
{
	/// TODO: fourier[非正交](sign) => fourier(sin) => fourier[正交](sign)
	uintptr_t quarter_cycle;
	uint32_t i;
	fourier->level = dsink_inner_dot_one(data, n) / fourier->cycle;
	for (i = 0; i < fourier->rank; ++i)
	{
		quarter_cycle = (fourier->cycle >> 2) / (i + 1);
		fourier->c[i].sa = (int32_t) (dsink_inner_dot_sin(data, n, quarter_cycle, 0) / (intptr_t) fourier->cycle);
		fourier->c[i].ca = (int32_t) (dsink_inner_dot_sin(data, n, quarter_cycle, quarter_cycle) / (intptr_t) fourier->cycle);
	}
}
