#ifndef _vkaa_function_h_
#define _vkaa_function_h_

#include "vkaa.h"

typedef vkaa_var_s* (*vkaa_function_f)(const vkaa_function_s *restrict r);

struct vkaa_function_s {
	vkaa_function_f function;
	vkaa_selector_s *selector;
	vkaa_var_s *this;
	vkaa_var_s *output;
	uintptr_t input_number;
	vkaa_var_s *input_list[];
};

#endif
