#include <phoneme/phoneme.h>
#include <math.h>
#include "const.define.h"
#include "../vya.common/random_const.inc"

static dyl_used phoneme_basefre_func(bf_random, refer_t)
{
	return pri?(arg->basefre * exp(random_const(pri))):arg->basefre;
}
dyl_export(bf_random, $basefre$vya.bf.random);

static dyl_used phoneme_arg2pri_func(bf_random_arg, refer_t)
{
	return random_const_arg(arg, vya_rg_normal, NULL);
}
dyl_export(bf_random_arg, $arg2pri$vya.bf.random.arg);
