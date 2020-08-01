#include <phoneme/phoneme.h>
#include "const.define.h"
#include "../vya.common/random_const.inc"

static dyl_used phoneme_details_func(df_randa, refer_t)
{
	if (d->used)
	{
		register double f;
		register uint32_t i;
		i = d->used;
		if (pri) while (i) d->saq[--i].sa = ((f = random_const(pri)) > 0)?f:0;
	}
}
dyl_export(df_randa, $details$vya.df.randa);

static dyl_used phoneme_arg2pri_func(df_randa_arg, refer_t)
{
	return random_const_arg(arg, vya_rg_normal, NULL);
}
dyl_export(df_randa_arg, $arg2pri$vya.df.randa.arg);

