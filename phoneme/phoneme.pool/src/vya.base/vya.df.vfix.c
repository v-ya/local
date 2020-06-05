#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <memory.h>
#include <alloca.h>
#include <math.h>

static float df_vfix_turn_xm(register note_details_s *restrict nd, float x)
{
	register uint32_t i;
	uint32_t n, m;
	float f, fa, k, ka, q;
	n = nd->used;
	m = 10;
	do {
		f = fa = 0;
		i = n;
		while (i)
		{
			ka = (k = i) * i;
			q = x * i;
			--i;
			k *= (float) nd->saq[i].sa;
			ka *= -(float) nd->saq[i].sa;
			q += (float) nd->saq[i].sq;
			f += k * cosf(q);
			fa += ka * sinf(q);
		}
		if (fa == 0) break;
		x -= (f /= fa);
	}
	while (--m && fabs(f) > 1e-5);
	return x;
}

static float df_vfix_4sampfre_xm(note_details_s *nd)
{
	float *s;
	float a, q, w, k;
	uint32_t i, j, n;
	n = nd->used * 4;
	q = 0;
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
			q = (float) nd->saq[j].sq;
			i = n;
			while (i)
			{
				--i;
				s[i] += a * sinf(w * i + q);
			}
		}
		q = w = 0;
		while (n)
		{
			if ((a = fabsf(s[--n])) > w)
			{
				w = a;
				q = k * n;
			}
		}
	}
	return q;
}

static dyl_used phoneme_details_func(df_vfix, refer_t)
{
	if (d->used)
	{
		double x, a, q;
		uint32_t i;
		x = df_vfix_turn_xm(d, df_vfix_4sampfre_xm(d));
		a = 0;
		i = d->used;
		while (i)
		{
			q = x * i;
			--i;
			a += d->saq[i].sa * sin(q + d->saq[i].sq);
		}
		a = 1.0 / a;
		i = d->used;
		while (i) d->saq[--i].sa *= a;
	}
}
dyl_export(df_vfix, $details$vya.df.vfix);

