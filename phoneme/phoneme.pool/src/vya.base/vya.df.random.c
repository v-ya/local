#include <phoneme/phoneme.h>
#include <math.h>
#include "const.define.h"
#include "../vya.common/random_const.inc"

static dyl_used phoneme_details_func(df_random, refer_t)
{
	if (d->used)
	{
		register uint32_t i;
		i = d->used;
		if (pri) while (i) d->saq[--i].sa *= exp(random_const(pri));
	}
}
dyl_export(df_random, $details$vya.df.random);

static dyl_used phoneme_arg2pri_func(df_random_arg, refer_t)
{
	return random_const_arg(arg, vya_rg_normal, NULL);
}
dyl_export(df_random_arg, $arg2pri$vya.df.random.arg);

