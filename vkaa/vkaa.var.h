#ifndef _vkaa_var_h_
#define _vkaa_var_h_

#include "vkaa.h"
#include <rbtree.h>

struct vkaa_var_s {
	uintptr_t type_id;
	const vkaa_type_s *type;
};

struct vkaa_vclear_s {
	rbtree_t *var;
};

vkaa_var_s* vkaa_var_initial(vkaa_var_s *restrict var, const vkaa_type_s *restrict type);
void vkaa_var_finally(vkaa_var_s *restrict var);

vkaa_selector_s* vkaa_var_find_selector(const vkaa_var_s *restrict var, const char *restrict name);

vkaa_vclear_s* vkaa_vclear_alloc(void);
vkaa_vclear_s* vkaa_vclear_push(vkaa_vclear_s *restrict vclear, vkaa_var_s *restrict var);

#endif
