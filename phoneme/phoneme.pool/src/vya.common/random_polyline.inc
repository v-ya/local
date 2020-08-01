#ifndef _vya_common_random_polyline_inc_
#define _vya_common_random_polyline_inc_

#include <refer.h>
#include <json.h>
#include <dylink.h>

double random_polyline(refer_t rp, double t);
dyl_alias(vya.common.random_polyline, random_polyline);
refer_t random_polyline_arg(json_inode_t *restrict arg, const char *restrict rgname, const char *restrict rsname);
dyl_alias(vya.common.random_polyline_arg, random_polyline_arg);

#endif
