#ifndef _vkaa_execute_h_
#define _vkaa_execute_h_

#include "vkaa.h"
#include <exbuffer.h>

struct vkaa_execute_s {
	exbuffer_t buffer;
	vkaa_function_s *const *execute_array;
	uintptr_t execute_number;
	vkaa_var_s *last_var;
};

vkaa_execute_s* vkaa_execute_alloc(void);
vkaa_execute_s* vkaa_execute_push(vkaa_execute_s *restrict exec, vkaa_function_s *restrict func);
vkaa_var_s* vkaa_execute_set_last(vkaa_execute_s *restrict exec, vkaa_var_s *restrict var);
vkaa_var_s* vkaa_execute_do(const vkaa_execute_s *restrict exec);
void vkaa_execute_clear(vkaa_execute_s *restrict exec);

#endif
