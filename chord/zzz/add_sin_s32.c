#include "_.h"

void chord_zzz_add_sin_s32(int32_t *restrict p, uintptr_t n, double a, double w, double q)
{
	uintptr_t i;
	a *= 0x7fffffff;
	for (i = 0; i < n; ++i)
		p[i] += (int32_t) a * sin(w * i + q);
}
