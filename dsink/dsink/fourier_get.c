#define _DEFAULT_SOURCE
#include "inner.h"
#include <alloca.h>
#include <math.h>

void dsink_inner_fourier_get(dsink_fourier_t *restrict fourier, const int32_t *restrict data, uintptr_t n)
{
	double *restrict sa;
	double *restrict ca;
	double k, w0, w1;
	uint32_t i;
	sa = (double *) alloca(sizeof(double) * fourier->rank) - 1;
	ca = (double *) alloca(sizeof(double) * fourier->rank) - 1;
	k = M_PI_2 / fourier->cycle;
	w0 = (M_PI * 2) / fourier->cycle;
	fourier->level = (int32_t) (dsink_inner_dot_one(data, n) / (int64_t) fourier->cycle);
	for (i = 1; i <= fourier->rank; ++i)
	{
		w1 = w0 * i;
		sa[i] = dsink_inner_dot_sin(data, n, w1) * k;
		ca[i] = dsink_inner_dot_cos(data, n, w1) * k;
	}
	dsink_inner_fourier_filter_sin2sign(++sa, ++ca, (uintptr_t) fourier->rank);
	for (i = 0; i < fourier->rank; ++i)
	{
		fourier->c[i].sa = (int32_t) sa[i];
		fourier->c[i].ca = (int32_t) ca[i];
	}
}
