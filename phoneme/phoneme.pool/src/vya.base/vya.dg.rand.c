#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <stdlib.h>
#include <math.h>

static dyl_used phoneme_details_func(dg_rand, refer_t)
{
	register uint32_t n;
	n = d->used = d->max;
	while (n)
	{
		--n;
		d->saq[n].sa = (1.0 / RAND_MAX) * rand();
		d->saq[n].sq = (M_PI * 2 / RAND_MAX) * rand();
	}
}
dyl_export(dg_rand, $details$vya.dg.rand);

