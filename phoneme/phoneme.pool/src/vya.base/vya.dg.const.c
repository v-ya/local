#include <phoneme/phoneme.h>

static dyl_used phoneme_details_func(dg_const, refer_t)
{
	register double _a;
	register uint32_t n;
	n = d->used = d->max;
	_a = 1.0 / n;
	while (n)
	{
		--n;
		d->saq[n].sa = _a;
		d->saq[n].sq = 0;
	}
}
dyl_export(dg_const, $details$vya.dg.const);

