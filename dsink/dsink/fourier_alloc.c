#include "inner.h"
#include <stdlib.h>

dsink_fourier_t* dsink_inner_fourier_alloc(uint32_t rank)
{
	dsink_fourier_t *restrict r;
	r = (dsink_fourier_t *) calloc(1, sizeof(dsink_fourier_t) + rank * sizeof(dsink_fourier_coefficient_t));
	if (r) r->rank = rank;
	return r;
}
