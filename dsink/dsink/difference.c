#include "inner.h"

void dsink_inner_difference(register const int32_t *restrict data, register int32_t *restrict diff, register uintptr_t n, register int32_t s)
{
	register uintptr_t i;
	register int32_t s1;
	for (i = 0; i < n; ++i)
	{
		diff[i] = (s1 = data[i]) - s;
		if (++i >= n) break;
		diff[i] = (s = data[i]) - s1;
	}
}
