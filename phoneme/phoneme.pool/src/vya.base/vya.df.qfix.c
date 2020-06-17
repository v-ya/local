#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <memory.h>
#include <alloca.h>
#include <math.h>
#include "../vya.common/q_normal.inc"

typedef struct df_qfix_data_s {
	double qsave;
	double ksave;
} df_qfix_data_s;

static double df_qfix_8sampfre_x0(register note_details_s *restrict nd, double x, double lk, double kk, double *rk)
{
	float *s;
	float a, q, w, k;
	double z, zm, r;
	uint32_t i, j, n;
	n = nd->used * 8;
	r = x;
	s = (float *) alloca(n * sizeof(float));
	if (s)
	{
		memset(s, 0, n * sizeof(float));
		j = nd->used;
		k = M_PI * 2 / n;
		while (j)
		{
			w = k * j;
			a = (float) nd->saq[--j].sa;
			q = (float) (nd->saq[j].sq + x);
			i = n;
			while (i)
			{
				--i;
				s[i] += a * sinf(w * i + q);
			}
		}
		zm = nd->used * nd->used;
		a = s[n - 1];
		for (i = 0; i < n; ++i)
		{
			if (a * s[i] <= 0 && (w = (s[i] - a) / k) * lk >= 0)
			{
				z = fabs(w - lk) * kk + fabs(q_normal(i * k - x));
				if (z < zm)
				{
					zm = z;
					r = i * k;
					*rk = w;
				}
			}
			a = s[i];
		}
	}
	return r;
}

static double df_qfix_turn_x0(register note_details_s *restrict nd, double x)
{
	uint32_t n, m;
	register uint32_t i;
	register double f, fa, q;
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
		register df_qfix_data_s *restrict save;
		register double x;
		register uint32_t i;
		if (!(save = *data))
		{
			*data = save = (df_qfix_data_s *) refer_alloz(sizeof(df_qfix_data_s));
			if (save)
			{
				x = df_qfix_8sampfre_x0(d, save->qsave, 0, 0, &save->ksave);
				save->qsave = df_qfix_turn_x0(d, x);
				goto label;
			}
		}
		if (save)
		{
			x = save->qsave + df_qfix_8sampfre_x0(d, save->qsave, save->ksave, 1, &save->ksave);
			save->qsave = x = df_qfix_turn_x0(d, x);
			label:
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

