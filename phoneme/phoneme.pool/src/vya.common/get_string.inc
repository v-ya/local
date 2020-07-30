#ifndef _vya_common_get_string_inc_
#define _vya_common_get_string_inc_

#include <json.h>
#include <dylink.h>

void get_string(const char *restrict *restrict r, json_inode_t *restrict arg, const char *restrict jpath);
dyl_alias(vya.common.get_string, get_string);

#endif
