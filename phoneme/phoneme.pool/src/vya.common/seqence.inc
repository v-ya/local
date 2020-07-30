#ifndef _vya_common_seqence_inc_
#define _vya_common_seqence_inc_

#include "value_deal.h"
#include <json.h>
#include <dylink.h>

double seqence(size_t n, double *restrict s, double *restrict v, double t, value_deal_f func, refer_t pri);
dyl_alias(vya.common.seqence, seqence);
void seqence_arg(size_t n, double *restrict s, double *restrict v, json_inode_t *arg);
dyl_alias(vya.common.seqence_arg, seqence_arg);

#endif
