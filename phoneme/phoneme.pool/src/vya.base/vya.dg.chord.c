#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <math.h>
#include "../vya.common/dg_a_qz.inc"

// link vya.df.chord

phoneme_details_func(df_chord, refer_t);
dyl_alias($details$vya.df.chord, df_chord);

static dyl_used phoneme_details_func(dg_chord, refer_t)
{
	d->used = d->max;
	dg_a_qz(d, 1);
	df_chord(pri, arg, d, data);
}
dyl_export(dg_chord, $details$vya.dg.chord);

