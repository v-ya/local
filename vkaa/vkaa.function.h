#ifndef _vkaa_function_h_
#define _vkaa_function_h_

#include "vkaa.h"

struct vkaa_function_s {
	vkaa_function_f function;
	const vkaa_selector_s *selector;
	vkaa_var_s *this;
	const vkaa_type_s *output_type;
	vkaa_var_s *output;
	uintptr_t input_number;
	vkaa_var_s *input_list[];
};

vkaa_function_s* vkaa_function_alloc(const vkaa_selector_s *restrict selector, vkaa_function_f function, const vkaa_type_s *restrict output_type, uintptr_t input_number, vkaa_var_s *const *restrict input_list);
void vkaa_function_set_this(vkaa_function_s *restrict func, vkaa_var_s *restrict this);
vkaa_function_s* vkaa_function_set_output(vkaa_function_s *restrict func, vkaa_var_s *restrict output);
vkaa_function_s* vkaa_function_set_input(vkaa_function_s *restrict func, uintptr_t index, vkaa_var_s *restrict input);
vkaa_var_s* vkaa_function_okay(vkaa_function_s *restrict func);

#endif
