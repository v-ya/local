#include <phoneme/phoneme.h>
#include "../vya.common/random_generate.h"

static dyl_used random_generate_func(rg_uniform, refer_t)
{
	// (rsrc - 0.5) * sqrt(12) * sigma + mu
	return (rsrc_generate(rsrc) - 0.5) * 3.464101615137754587 * sigma + mu;
}
dyl_export(rg_uniform, vya.rg.uniform);
