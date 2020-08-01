#include <phoneme/phoneme.h>
#include <math.h>
#include "const.define.h"
#include "../vya.common/random_polyline.inc"

static dyl_used phoneme_details_func(df_r_af_polyline, refer_t)
{
	if (d->used)
	{
		register double bf, k;
		register uint32_t i;
		bf = arg->basefre;
		i = d->used;
		if (pri) while (i)
		{
			k = exp(random_polyline(pri, (double) i * bf));
			d->saq[--i].sa *= k;
		}
	}
}
dyl_export(df_r_af_polyline, $details$vya.df.r_af_polyline);

static dyl_used phoneme_arg2pri_func(df_r_af_polyline_arg, refer_t)
{
	return random_polyline_arg(arg, vya_rg_normal, NULL);
}
dyl_export(df_r_af_polyline_arg, $arg2pri$vya.df.r_af_polyline.arg);

