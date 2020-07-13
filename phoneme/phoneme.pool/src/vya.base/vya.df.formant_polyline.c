#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <math.h>
#include "../vya.common/polyline.inc"

typedef struct df_formant_polyline_a_t {
	refer_t u;
	refer_t a;
	refer_t c;
} df_formant_polyline_a_t;

typedef struct df_formant_polyline_s {
	size_t n;
	df_formant_polyline_a_t a[];
} df_formant_polyline_s;

static dyl_used phoneme_details_func(df_formant_polyline, df_formant_polyline_s*restrict)
{
	register df_formant_polyline_a_t *restrict p;
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
			_c = arg->t;
			_u = polyline(_c, p->u) * _k;
			_a = polyline(_c, p->a);
			_c = polyline(_c, p->c) * _k;
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
dyl_export(df_formant_polyline, $details$vya.df.formant_polyline);

static void df_formant_polyline_free_func(df_formant_polyline_s *restrict r)
{
	register size_t n;
	register df_formant_polyline_a_t *p;
	for (n = r->n, p = r->a; n; --n)
	{
		if (p->u) refer_free(p->u);
		if (p->a) refer_free(p->a);
		if (p->c) refer_free(p->c);
		++p;
	}
}

static dyl_used phoneme_arg2pri_func(df_formant_polyline_arg, df_formant_polyline_s*)
{
	df_formant_polyline_s *r;
	json_inode_t *v, *pl;
	size_t n;
	if (arg && arg->type == json_inode_array && (n = arg->value.array.number))
	{
		r = (df_formant_polyline_s *) refer_alloz(sizeof(df_formant_polyline_s) + n * sizeof(df_formant_polyline_a_t));
		if (r)
		{
			r->n = n;
			refer_set_free(r, (refer_free_f) df_formant_polyline_free_func);
			for (n = 0; n < r->n; ++n)
			{
				v = json_array_find(arg, n);
				if (!(pl = json_object_find(v, "u"))) goto Err;
				if (!(r->a[n].u = polyline_arg(pl, NULL, NULL))) goto Err;
				if (!(pl = json_object_find(v, "a"))) goto Err;
				if (!(r->a[n].a = polyline_arg(pl, NULL, NULL))) goto Err;
				if (!(pl = json_object_find(v, "c"))) goto Err;
				if (!(r->a[n].c = polyline_arg(pl, NULL, NULL))) goto Err;
			}
			return r;
			Err:
			refer_free(r);
		}
	}
	return NULL;
}
dyl_export(df_formant_polyline_arg, $arg2pri$vya.df.formant_polyline.arg);

