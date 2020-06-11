#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <math.h>
#include "../vya.common/get_float.inc"

typedef struct df_grapow_s {
	double a;
	double b;
} df_grapow_s;

static dyl_used phoneme_details_func(df_grapow, df_grapow_s*restrict)
{
	register note_details_saq_s *restrict saq;
	register uint32_t i;
	register double p, k;
	if (pri)
	{
		p = arg->t;
		p = pri->a * (1 - p) + pri->b * p;
		i = d->used;
		saq = d->saq;
		while (i)
		{
			k = pow(i, p);
			--i;
			saq[i].sa *= k;
		}
	}
}
dyl_export(df_grapow, $details$vya.df.grapow);

static dyl_used phoneme_arg2pri_func(df_grapow_arg, df_grapow_s*)
{
	register df_grapow_s *r;
	r = NULL;
	r = (df_grapow_s *) refer_alloz(sizeof(df_grapow_s));
	if (r)
	{
		r->a = 0;
		get_float(&r->a, arg, ".a");
		r->b = r->a;
		get_float(&r->b, arg, ".b");
	}
	return r;
}
dyl_export(df_grapow_arg, $arg2pri$vya.df.grapow.arg);

