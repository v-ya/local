#ifndef _vya_common_rgf_find_inc_
#define _vya_common_rgf_find_inc_

#include "random_generate.h"
#include <phoneme/phoneme_arg.h>
#include <dylink.h>

random_generate_f rgf_find(const char *restrict sym, phoneme_arg2pri_f *restrict arg2pri);
dyl_alias(vya.common.rgf_find, rgf_find);

#endif
