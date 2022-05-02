#ifndef _vkaa_function_h_
#define _vkaa_function_h_

#include "vkaa.h"

struct vkaa_function_s {
	vkaa_function_f function;
	vkaa_selector_s *selector;
	vkaa_var_s *this;
	vkaa_var_s *output;
	uintptr_t output_typeid;
	uintptr_t input_number;
	vkaa_var_s *input_list[];
};

#endif
