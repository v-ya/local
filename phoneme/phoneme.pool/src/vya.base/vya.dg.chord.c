#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <math.h>

static dyl_used phoneme_details_func(dg_chord, refer_t)
{
	static double k = 6 / M_PI / M_PI;
	register double j;
	register uint32_t n;
	n = d->used = d->max;
	while (n)
	{
		j = 1.0 / n;
		--n;
		d->saq[n].sa = k * j * j;
		d->saq[n].sq = 0;
	}
}
dyl_export(dg_chord, $details$vya.dg.chord);

