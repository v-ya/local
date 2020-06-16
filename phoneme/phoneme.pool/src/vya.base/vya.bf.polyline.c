#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include "../vya.common/seqence.inc"

typedef struct bf_polyline_s {
	size_t n;
	double *s;
	double *v;
	double value[];
} bf_polyline_s;

static dyl_used phoneme_basefre_func(bf_polyline, bf_polyline_s*restrict)
{
	if (pri) return arg->basefre * seqence(pri->n, pri->s, pri->v, arg->t);
	return arg->basefre;
}
dyl_export(bf_polyline, $basefre$vya.bf.polyline);

static dyl_used phoneme_arg2pri_func(bf_polyline_arg, bf_polyline_s*)
{
	bf_polyline_s *r;
	size_t n;
	r = NULL;
	if (arg && arg->type == json_inode_array && (n = arg->value.array.number))
	{
		r = refer_alloz(sizeof(bf_polyline_s) + sizeof(double) * 2 * n);
		if (r)
		{
			seqence_arg(r->n = n, r->s = r->value, r->v = r->value + n, arg);
			while (n)
			{
				--n;
				if (r->v[n] < 0.1) r->v[n] = 0.1;
				if (r->v[n] > 10) r->v[n] = 10;
			}
		}
	}
	return r;
}
dyl_export(bf_polyline_arg, $arg2pri$vya.bf.polyline.arg);

