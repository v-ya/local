#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <math.h>

static dyl_used double q_normal(register double r)
{
	r = fmod(r, M_PI * 2);
	if (r < 0) r += M_PI * 2;
	return r;
}
dyl_export(q_normal, vya.common.q_normal);

