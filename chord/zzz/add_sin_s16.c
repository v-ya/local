#include "_.h"

void chord_zzz_add_sin_s16(int16_t *restrict p, uintptr_t n, float a, float w, float q)
{
	uintptr_t i;
	a *= chord_zzz_s16_max;
	for (i = 0; i < n; ++i)
		p[i] += (int16_t) (a * sinf(w * i + q));
}
