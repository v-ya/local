#include "inner.h"
#include <stdlib.h>

void dsink_inner_fourier_free(dsink_fourier_t *restrict fourier)
{
	free(fourier);
}
