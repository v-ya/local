#include <phoneme/phoneme.h>
#include <math.h>
#include "common.h"

typedef struct e_weaken_s {
	double u;
	double p;
	double k;
} e_weaken_s;

static dyl_used phoneme_envelope_func(e_weaken, e_weaken_s*)
{
	if (pri && t >= 0 && t <= 1)
	{
		return volume * pri->k * pow(t, pri->u) * pow(1 - t, 1 - pri->u) * exp(- pri->p * t);
	}
	return 0;
}
dyl_export(e_weaken, vya$envelope$e.weaken);

static dyl_used phoneme_arg2pri_func(e_weaken_arg, e_weaken_s*)
{
	e_weaken_s *r;
	double v;
	r = refer_alloc(sizeof(e_weaken_s));
	if (r)
	{
		v = 0.2;
		json_set_float(&v, arg, ".u");
		if (v < 0) v = 0;
		if (v > 1) v = 1;
		r->u = v;
		v = 1;
		json_set_float(&v, arg, ".p");
		if (v < 1) v = 1;
		r->p = v;
		// find max
		v += 1;
		v = v * v - 4 * r->u * r->p;
		if (v > 0) v = sqrt(v);
		else v = 0;
		v = (r->p + 1 - v) / 2 / r->p;
		if (v < 0) v = 0;
		if (v > 1) v = 1;
		r->k = 1;
		r->k /= e_weaken(r, v, 1, 0);
	}
	return r;
}
dyl_export(e_weaken_arg, vya$arg2pri$e.weaken.arg);
