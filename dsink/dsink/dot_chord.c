#define _GNU_SOURCE
#include "inner.h"
#include <math.h>

double dsink_inner_dot_chord(register const int32_t *restrict data, register uintptr_t n, register double w, double *restrict q)
{
	register double rs, rc;
	double s, c;
	rs = rc = 0;
	for (register uintptr_t i = 0; i < n; ++i)
	{
		sincos(w * i, &s, &c);
		rs += s * data[i];
		rc += c * data[i];
	}
	if (q) *q = atan2(rc, rs);
	return sqrt(rs * rs + rc * rc);
}
