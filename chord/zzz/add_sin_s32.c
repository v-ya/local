#include "_.h"

void chord_zzz_add_sin_s32(int32_t *restrict p, uintptr_t n, double a, double w, double q)
{
	uintptr_t i;
	a *= chord_zzz_s32_max;
	for (i = 0; i < n; ++i)
		p[i] += (int32_t) (a * sin(w * i + q));
}
