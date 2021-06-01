#include "inner.h"

void dsink_inner_fourier_add(int32_t *restrict data, uintptr_t n, const dsink_fourier_t *restrict fourier)
{
	uintptr_t quarter_cycle;
	uint32_t i;
	dsink_inner_add_one(data, n, fourier->level);
	for (i = 0; i < fourier->rank; ++i)
	{
		quarter_cycle = (fourier->cycle >> 2) / (i + 1);
		dsink_inner_add_sin(data, n, quarter_cycle, 0, fourier->c[i].sa);
		dsink_inner_add_sin(data, n, quarter_cycle, quarter_cycle, fourier->c[i].ca);
	}
}
