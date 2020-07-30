#ifndef _vya_common_get_float_inc_
#define _vya_common_get_float_inc_

#include <json.h>
#include <dylink.h>

void get_float(double *restrict r, json_inode_t *restrict arg, const char *restrict jpath);
dyl_alias(vya.common.get_float, get_float);

#endif
