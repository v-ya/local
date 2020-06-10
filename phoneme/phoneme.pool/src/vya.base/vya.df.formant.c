#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <math.h>
#include "../vya.common/get_float.inc"

typedef struct df_formant_a_t {
	double u;
	double a;
	double c;
} df_formant_a_t;

typedef struct df_formant_s {
	size_t n;
	df_formant_a_t a[];
} df_formant_s;

static dyl_used phoneme_details_func(df_formant, df_formant_s*restrict)
{
	register df_formant_a_t *restrict p;
	register size_t n;
	register note_details_saq_s *restrict saq;
	register uint32_t i;
	register double _u, _a, _c, _k;
	if (pri && (n = pri->n))
	{
		p = pri->a;
		saq = d->saq;
		while (n)
		{
			_k = 1 / arg->f;
			_u = p->u * _k;
			_a = p->a;
			_c = p->c * _k;
			i = d->used;
			while (i)
			{
				_k = (i - _u) / _c;
				_k = 1 + _a * exp((-0.5) * _k * _k);
				--i;
				saq[i].sa *= _k;
			}
			++p;
			--n;
		}
	}
}
dyl_export(df_formant, $details$vya.df.formant);

static dyl_used phoneme_arg2pri_func(df_formant_arg, df_formant_s*)
{
	df_formant_s *r;
	json_inode_t *v;
	double u, a, c;
	size_t n;
	r = NULL;
	if (arg && arg->type == json_inode_array && (n = arg->value.array.number))
	{
		r = (df_formant_s *) refer_alloz(sizeof(df_formant_s) + n * sizeof(df_formant_a_t));
		if (r)
		{
			r->n = n;
			u = a = 0;
			c = 1;
			for (n = 0; n < r->n; ++n)
			{
				v = json_array_find(arg, n);
				get_float(&u, v, ".u");
				get_float(&c, v, ".c");
				get_float(&a, v, ".a");
				if (c == 0) c = 1;
				r->a[n].u = u;
				r->a[n].c = c;
				r->a[n].a = a;
			}
		}
	}
	return r;
}
dyl_export(df_formant_arg, $arg2pri$vya.df.formant.arg);

