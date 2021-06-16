#include "inner.h"
#include <math.h>

double dsink_inner_dot_cos(register const int32_t *restrict data, register uintptr_t n, register double w)
{
	register double r = 0;
	for (register uintptr_t i = 0; i < n; ++i)
		r += cos(w * i) * data[i];
	return r;
}
