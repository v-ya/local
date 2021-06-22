#include "_.h"

void chord_zzz_add_sin_f64(double *restrict p, uintptr_t n, double a, double w, double q)
{
	uintptr_t i;
	for (i = 0; i < n; ++i)
		p[i] += a * sin(w * i + q);
}
