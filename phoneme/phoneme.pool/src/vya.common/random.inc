#ifndef _vya_common_random_inc_
#define _vya_common_random_inc_

#include <refer.h>
#include <json.h>
#include <dylink.h>

double random(refer_t r, double mu, double sigma);
dyl_alias(vya.common.random, random);
refer_t random_arg(json_inode_t *restrict arg, const char *restrict rgname, const char *restrict rsname);
dyl_alias(vya.common.random_arg, random_arg);

#endif
