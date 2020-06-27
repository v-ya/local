#include <phoneme/phoneme.h>
#include <math.h>
#include "../pthis.h"
#include "../vya.common/value_deal.h"
#include "../vya.common/get_float.inc"

typedef struct vd_frestep_s {
	double step_rec;
} vd_frestep_s;

static dyl_used value_deal_func(vd_frestep, register vd_frestep_s*restrict)
{
	if (pri && pri->step_rec)
		value *= pri->step_rec;
	else value /= pthis_phoneme_script->base_fre_step;
	return exp2(value);
}
dyl_export(vd_frestep, vya.vd.frestep);

static dyl_used phoneme_arg2pri_func(vd_frestep_arg, vd_frestep_s*)
{
	register vd_frestep_s *restrict r;
	double step;
	r = refer_alloz(sizeof(vd_frestep_s));
	if (r)
	{
		step = pthis_phoneme_script->base_fre_step;
		get_float(&step, arg, ".step");
		if (step) r->step_rec = 1.0 / step;
	}
	return r;
}
dyl_export(vd_frestep_arg, vya.vd.frestep.arg);
