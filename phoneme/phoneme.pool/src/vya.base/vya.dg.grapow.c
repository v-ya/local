#include <phoneme/phoneme.h>
#include "../vya.common/dg_a_qz.inc"

// link vya.df.grapow

phoneme_details_func(df_grapow, refer_t);
dyl_alias($details$vya.df.grapow, df_grapow);

static dyl_used phoneme_details_func(dg_grapow, refer_t)
{
	d->used = d->max;
	dg_a_qz(d, 1);
	df_grapow(pri, arg, d);
}
dyl_export(dg_grapow, $details$vya.dg.grapow);

dyl_alias($arg2pri$vya.df.grapow.arg, $arg2pri$vya.dg.grapow.arg);

