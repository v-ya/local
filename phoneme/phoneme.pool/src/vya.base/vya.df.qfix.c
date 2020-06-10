#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <math.h>
#include "../vya.common/q_normal.inc"

static double df_qfix_turn_x0(register note_details_s *restrict nd, double x)
{
	register uint32_t i;
	uint32_t n, m;
	double f, fa, k, ka, q;
	n = nd->used;
	m = 10;
	do {
		f = fa = 0;
		i = n;
		while (i)
		{
			--i;
			ka = (k = nd->saq[i].sa) * (q = nd->saq[i].sq);
			q *= x;
			f += k * sin(q);
			fa += ka * cos(q);
		}
		if (fa == 0)
		{
			if (fabs(f) > 1e-5)
				x = 0;
			break;
		}
		x -= (f /= fa);
	}
	while (--m && fabs(f) > 1e-5);
	return x;
}

static dyl_used phoneme_details_func(df_qfix, refer_t)
{
	if (d->used)
	{
		register double k;
		register uint32_t i;
		i = d->used;
		while (i)
		{
			--i;
			d->saq[i].sq = q_normal(d->saq[i].sq);
		}
		k = df_qfix_turn_x0(d, 1);
		i = d->used;
		while (i)
		{
			--i;
			d->saq[i].sq *= k;
		}
	}
}
dyl_export(df_qfix, $details$vya.df.qfix);

