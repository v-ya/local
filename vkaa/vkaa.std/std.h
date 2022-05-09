#ifndef _vkaa_std_std_h_
#define _vkaa_std_std_h_

#include "../vkaa.h"
#include "../vkaa.std.h"
#include "../vkaa.std.extern.h"

vkaa_oplevel_s* vkaa_std_create_oplevel(void);
vkaa_std_typeid_s* vkaa_std_typeid_alloc(vkaa_tpool_s *restrict tpool);
vkaa_tpool_s* vkaa_std_create_tpool(vkaa_std_typeid_s *restrict *restrict typeid);
vkaa_parse_s* vkaa_std_create_parse(const vkaa_oplevel_s *restrict oplevel, const vkaa_std_typeid_s *restrict typeid);

#endif
