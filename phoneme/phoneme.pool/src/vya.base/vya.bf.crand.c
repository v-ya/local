#include <phoneme/phoneme.h>
#include <stdlib.h>
#include "../vya.common/get_float.inc"

typedef struct bf_crand_s {
	double kr;
} bf_crand_s;

static dyl_used phoneme_basefre_func(bf_crand, bf_crand_s*restrict)
{
	if (pri)
	{
		return arg->basefre * (1 + (pri->kr * (rand() * (2.0 / RAND_MAX) - 1)));
	}
	return arg->basefre;
}
dyl_export(bf_crand, $basefre$vya.bf.crand);

static dyl_used phoneme_arg2pri_func(bf_crand_arg, bf_crand_s*)
{
	register bf_crand_s *r;
	r = (bf_crand_s *) refer_alloc(sizeof(bf_crand_s));
	if (r)
	{
		r->kr = 0;
		get_float(&r->kr, arg, ".kr");
		if (r->kr < 0) r->kr = 0;
		if (r->kr > 1) r->kr = 1;
	}
	return r;
}
dyl_export(bf_crand_arg, $arg2pri$vya.bf.crand.arg);


