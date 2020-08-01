#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include "const.define.h"
#include "../vya.common/polyline.inc"

static dyl_used phoneme_basefre_func(bf_polyline, refer_t)
{
	register double v;
	v = 1;
	if (pri)
	{
		v = polyline(arg->t, pri);
		if (v <= 0) v = 1;
	}
	return arg->basefre * v;
}
dyl_export(bf_polyline, $basefre$vya.bf.polyline);

static dyl_used phoneme_arg2pri_func(bf_polyline_arg, refer_t)
{
	return polyline_arg(arg, vya_vd_frestep, NULL);
}
dyl_export(bf_polyline_arg, $arg2pri$vya.bf.polyline.arg);

