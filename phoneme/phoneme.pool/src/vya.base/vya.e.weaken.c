#include <phoneme/phoneme.h>
#include <math.h>
#include "../vya.common/get_float.inc"

typedef struct e_weaken_s {
	double u;
	double p;
	double k;
} e_weaken_s;

static dyl_used phoneme_envelope_func(e_weaken, e_weaken_s*restrict)
{
	register double t;
	t = arg->t;
	if (pri && t >= 0 && t <= 1)
	{
		return arg->volume * pri->k * pow(t, pri->u) * pow(1 - t, 1 - pri->u) * exp(- pri->p * t);
	}
	return 0;
}
dyl_export(e_weaken, $envelope$vya.e.weaken);

static dyl_used phoneme_arg2pri_func(e_weaken_arg, e_weaken_s*)
{
	e_weaken_s *r;
	double v;
	r = refer_alloc(sizeof(e_weaken_s));
	if (r)
	{
		v = 0.2;
		get_float(&v, arg, ".u");
		if (v < 0) v = 0;
		if (v > 1) v = 1;
		r->u = v;
		v = 1;
		get_float(&v, arg, ".p");
		r->p = v;
		// find max
		if (r->p)
		{
			v += 1;
			v = v * v - 4 * r->u * r->p;
			if (v > 0) v = sqrt(v);
			else v = 0;
			v = (r->p + 1 - v) / (r->p * 2);
			if (v < 0) v = 0;
			if (v > 1) v = 1;
		}
		else v = r->u;
		v = pow(v, r->u) * pow(1 - v, 1 - r->u) * exp(- r->p * v);
		r->k = 1 / v;
	}
	return r;
}
dyl_export(e_weaken_arg, $arg2pri$vya.e.weaken.arg);
