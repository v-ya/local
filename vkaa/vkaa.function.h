#ifndef _vkaa_function_h_
#define _vkaa_function_h_

#include "vkaa.h"

struct vkaa_function_s {
	vkaa_function_f function;
	const vkaa_selector_s *selector;
	vkaa_var_s *this;
	vkaa_var_s *output;
	uintptr_t output_typeid;
	uintptr_t input_number;
	vkaa_var_s *input_list[];
};

vkaa_function_s* vkaa_function_alloc(const vkaa_selector_s *restrict selector, const vkaa_selector_param_t *restrict param, const vkaa_selector_rdata_t *restrict rdata);
vkaa_function_s* vkaa_function_okay(vkaa_function_s *restrict func, const vkaa_tpool_s *restrict tpool);

#endif
