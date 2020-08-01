#include <phoneme/phoneme.h>
#include "const.define.h"
#include "../vya.common/random_const.inc"

static dyl_used phoneme_basefre_func(bf_crand, refer_t)
{
	if (pri)
	{
		register double k;
		if ((k = 1 + random_const(pri)) < 0) k = 0;
		return arg->basefre * k;
	}
	return arg->basefre;
}
dyl_export(bf_crand, $basefre$vya.bf.crand);

static dyl_used phoneme_arg2pri_func(bf_crand_arg, refer_t)
{
	return random_const_arg(arg, vya_rg_normal, NULL);
}
dyl_export(bf_crand_arg, $arg2pri$vya.bf.crand.arg);
