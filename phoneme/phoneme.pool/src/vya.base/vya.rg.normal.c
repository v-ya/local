
#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <math.h>
#include "../vya.common/random_generate.h"

static dyl_used random_generate_func(rg_normal, refer_t)
{
	// n = sqrt(-2 * ln(rsrc)) * sin(2 * pi * rsrc)
	// = n * sigma + mu
	return sigma * sqrt(-2 * log(rsrc_generate(rsrc))) * sin((M_PI * 2) * rsrc_generate(rsrc)) + mu;
}
dyl_export(rg_normal, vya.rg.normal);
