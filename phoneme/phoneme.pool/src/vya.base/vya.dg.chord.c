#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <math.h>

static dyl_used phoneme_details_func(dg_chord, refer_t)
{
	static double k = 6 / M_PI / M_PI;
	uint32_t i, n;
	n = d->used = d->max;
	i = 0;
	while (i < n)
	{
		d->saq[i].sa = k / (i + 1) / (i + 1);
		d->saq[i].sq = 0;
		++i;
	}
}
dyl_export(dg_chord, vya$details$dg.chord);

