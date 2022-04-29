#ifndef _vkaa_std_std_h_
#define _vkaa_std_std_h_

#include "../vkaa.h"
#include "../vkaa.std.h"

vkaa_oplevel_s* vkaa_std_create_oplevel(void);
vkaa_tpool_s* vkaa_std_create_tpool(vkaa_std_typeid_t *restrict typeid);

#endif
