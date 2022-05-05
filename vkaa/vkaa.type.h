#ifndef _vkaa_type_h_
#define _vkaa_type_h_

#include "vkaa.h"
#include <vattr.h>

typedef vkaa_var_s* (*vkaa_type_create_f)(const vkaa_type_s *restrict type);

struct vkaa_type_s {
	uintptr_t id;
	const char *name;
	vattr_s *selector;
	vkaa_type_create_f create;
};

vkaa_type_s* vkaa_type_initial(vkaa_type_s *restrict type, uintptr_t id, const char *name, vkaa_type_create_f create);
void vkaa_type_finally(vkaa_type_s *restrict type);

vkaa_selector_s* vkaa_type_find_selector(const vkaa_type_s *restrict type, const char *restrict name);
vkaa_selector_s* vkaa_type_insert_selector(const vkaa_type_s *restrict type, const char *restrict name, vkaa_selector_s *restrict selector);

#endif
