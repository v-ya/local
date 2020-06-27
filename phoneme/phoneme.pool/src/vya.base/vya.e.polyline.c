#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include "../vya.common/polyline.inc"

static dyl_used phoneme_envelope_func(e_polyline, refer_t)
{
	register double v;
	v = 1;
	if (pri)
	{
		v = polyline(arg->t, pri);
		if (v < 0) v = 0;
	}
	return arg->volume * v;
}
dyl_export(e_polyline, $envelope$vya.e.polyline);

static dyl_used phoneme_arg2pri_func(e_polyline_arg, refer_t)
{
	return polyline_arg(arg, NULL, NULL);
}
dyl_export(e_polyline_arg, $arg2pri$vya.e.polyline.arg);

