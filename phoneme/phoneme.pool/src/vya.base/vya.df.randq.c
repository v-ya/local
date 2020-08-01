#include <phoneme/phoneme.h>
#include "const.define.h"
#include "../vya.common/random_const.inc"

static dyl_used phoneme_details_func(df_randq, refer_t)
{
	if (d->used)
	{
		register uint32_t i;
		i = d->used;
		if (pri) while (i) d->saq[--i].sq = random_const(pri);
	}
}
dyl_export(df_randq, $details$vya.df.randq);

static dyl_used phoneme_arg2pri_func(df_randq_arg, refer_t)
{
	return random_const_arg(arg, vya_rg_normal, NULL);
}
dyl_export(df_randq_arg, $arg2pri$vya.df.randq.arg);

