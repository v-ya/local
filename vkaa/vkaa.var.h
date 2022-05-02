#ifndef _vkaa_var_h_
#define _vkaa_var_h_

#include "vkaa.h"
#include <hashmap.h>

struct vkaa_var_s {
	uintptr_t type_id;
	const vkaa_type_s *type;
	void *value;
};

vkaa_var_s* vkaa_var_initial(vkaa_var_s *restrict var, const vkaa_type_s *restrict type, void *value);
void vkaa_var_finally(vkaa_var_s *restrict var);

#endif
