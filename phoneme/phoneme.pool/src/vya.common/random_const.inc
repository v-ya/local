#ifndef _vya_common_random_const_inc_
#define _vya_common_random_const_inc_

#include <refer.h>
#include <json.h>
#include <dylink.h>

double random_const(refer_t rc);
dyl_alias(vya.common.random_const, random_const);
refer_t random_const_arg(json_inode_t *restrict arg, const char *restrict rgname, const char *restrict rsname);
dyl_alias(vya.common.random_const_arg, random_const_arg);

#endif
