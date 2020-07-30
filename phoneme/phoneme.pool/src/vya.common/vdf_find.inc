#ifndef _vya_common_vdf_find_inc_
#define _vya_common_vdf_find_inc_

#include "value_deal.h"
#include <phoneme/phoneme_arg.h>
#include <dylink.h>

value_deal_f vdf_find(const char *restrict sym, phoneme_arg2pri_f *restrict arg2pri);
dyl_alias(vya.common.vdf_find, vdf_find);

#endif
