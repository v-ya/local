#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include "../vya.common/seqence.inc"

typedef struct e_polyline_s {
	size_t n;
	double *s;
	double *v;
	double value[];
} e_polyline_s;

static dyl_used phoneme_envelope_func(e_polyline, e_polyline_s*restrict)
{
	if (pri) return arg->volume * seqence(pri->n, pri->s, pri->v, arg->t);
	return arg->volume;
}
dyl_export(e_polyline, $envelope$vya.e.polyline);

static dyl_used phoneme_arg2pri_func(e_polyline_arg, e_polyline_s*)
{
	e_polyline_s *r;
	size_t n;
	r = NULL;
	if (arg && arg->type == json_inode_array && (n = arg->value.array.number))
	{
		r = refer_alloz(sizeof(e_polyline_s) + sizeof(double) * 2 * n);
		if (r)
		{
			seqence_arg(r->n = n, r->s = r->value, r->v = r->value + n, arg);
			while (n)
			{
				--n;
				if (r->v[n] < 0) r->v[n] = 0;
				if (r->v[n] > 1) r->v[n] = 1;
			}
		}
	}
	return r;
}
dyl_export(e_polyline_arg, $arg2pri$vya.e.polyline.arg);

