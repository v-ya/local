#ifndef _vya_common_polyline_inc_
#define _vya_common_polyline_inc_

#include <refer.h>
#include <json.h>
#include <dylink.h>

double polyline(double t, refer_t p);
dyl_alias(vya.common.polyline, polyline);
refer_t polyline_arg(json_inode_t *restrict arg, const char *restrict asym, const char *restrict bsym);
dyl_alias(vya.common.polyline_arg, polyline_arg);

#endif
