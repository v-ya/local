#ifndef _vya_common_seqarray_inc_
#define _vya_common_seqarray_inc_

#include <refer.h>
#include <json.h>
#include <dylink.h>

double seqarray(refer_t sa, double x);
dyl_alias(vya.common.seqarray, seqarray);
refer_t seqarray_alloc(double *restrict value, size_t number, double start, double length);
dyl_alias(vya.common.seqarray_alloc, seqarray_alloc);
refer_t seqarray_arg(json_inode_t *arg, double start, double length);
dyl_alias(vya.common.seqarray_arg, seqarray_arg);

#endif
