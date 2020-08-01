#include <phoneme/phoneme.h>
#include <math.h>
#include "const.define.h"
#include "../vya.common/random_polyline.inc"

static dyl_used phoneme_basefre_func(bf_random_polyline, refer_t)
{
	return pri?(arg->basefre * exp(random_polyline(pri, arg->t))):arg->basefre;
}
dyl_export(bf_random_polyline, $basefre$vya.bf.random_polyline);

static dyl_used phoneme_arg2pri_func(bf_random_polyline_arg, refer_t)
{
	return random_polyline_arg(arg, vya_rg_normal, NULL);
}
dyl_export(bf_random_polyline_arg, $arg2pri$vya.bf.random_polyline.arg);
