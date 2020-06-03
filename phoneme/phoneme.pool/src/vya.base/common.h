#ifndef _common_h_
#define _common_h_

#include <phoneme/phoneme.h>

void json_set_float(double *r, json_inode_t *arg, const char *restrict jpath);
dyl_alias(vya.common.json_set_float, json_set_float);

#endif
