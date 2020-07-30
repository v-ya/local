#ifndef _vya_common_random_generate_inc_
#define _vya_common_random_generate_inc_

#include "random_generate.h"
#include <dylink.h>

random_src_s* rsrc_alloc(json_inode_t *restrict arg, const char *restrict name);
dyl_alias(vya.common.rsrc_alloc, rsrc_alloc);

#endif
