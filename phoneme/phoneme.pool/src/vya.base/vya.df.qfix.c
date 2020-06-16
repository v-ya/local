#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <math.h>
#include "../vya.common/q_normal.inc"

static double df_qfix_turn_x0(register note_details_s *restrict nd, double x)
{
	register uint32_t i;
	uint32_t n, m;
	double f, fa, q;
	n = nd->used;
	m = 10;
	do {
		f = fa = 0;
		i = n;
		while (i)
		{
			--i;
			q = nd->saq[i].sq + x;
			f += sin(q);
			fa += cos(q);
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
		register double *restrict qsave;
		register double x;
		register uint32_t i;
		if (!(qsave = *data))
		{
			*data = qsave = (double *) refer_alloz(sizeof(double));
		}
		if (qsave)
		{
			*qsave = x = df_qfix_turn_x0(d, *qsave);
			i = d->used;
			while (i)
			{
				--i;
				d->saq[i].sq += x;
			}
		}
	}
}
dyl_export(df_qfix, $details$vya.df.qfix);

