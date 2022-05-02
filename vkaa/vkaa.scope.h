#ifndef _vkaa_scope_h_
#define _vkaa_scope_h_

#include "vkaa.h"
#include <hashmap.h>

struct vkaa_scope_s {
	vkaa_scope_s *parent;
	uintptr_t level;
	hashmap_t var;
};

vkaa_scope_s* vkaa_scope_alloc(vkaa_scope_s *restrict parent);
vkaa_var_s* vkaa_scope_find(const vkaa_scope_s *restrict scope, const char *restrict name);
vkaa_var_s* vkaa_scope_put(vkaa_scope_s *restrict scope, const char *restrict name, vkaa_var_s *var);
vkaa_var_s* vkaa_scope_set(vkaa_scope_s *restrict scope, const char *restrict name, vkaa_var_s *var);
void vkaa_scope_unset(vkaa_scope_s *restrict scope, const char *restrict name);

#endif
