#ifndef _vkaa_selector_h_
#define _vkaa_selector_h_

#include "vkaa.h"

struct vkaa_selector_param_t {
	const char *name;
	vkaa_execute_s *exec;
	vkaa_var_s *this;
	vkaa_var_s *const *input_list;
	uintptr_t input_number;
};

struct vkaa_selector_rdata_t {
	vkaa_function_f function;
	uintptr_t output_typeid;
	vkaa_var_s *output_must;
};

typedef vkaa_selector_rdata_t* (*vkaa_selector_f)(const vkaa_selector_s *restrict r, const vkaa_selector_param_t *restrict param, vkaa_selector_rdata_t *restrict rdata);

struct vkaa_selector_s {
	vkaa_selector_f selector;
};

#endif
