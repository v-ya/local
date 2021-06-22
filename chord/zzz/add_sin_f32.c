#include "_.h"

void chord_zzz_add_sin_f32(float *restrict p, uintptr_t n, float a, float w, float q)
{
	uintptr_t i;
	for (i = 0; i < n; ++i)
		p[i] += a * sinf(w * i + q);
}
