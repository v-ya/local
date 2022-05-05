#ifndef _vkaa_selector_h_
#define _vkaa_selector_h_

#include "vkaa.h"

typedef struct vkaa_selector_param_t {
	const vkaa_tpool_s *tpool;
	vkaa_execute_s *exec;
	vkaa_var_s *this;
	vkaa_var_s *const *input_list;
	uintptr_t input_number;
} vkaa_selector_param_t;

typedef vkaa_function_s* (*vkaa_selector_f)(const vkaa_selector_s *restrict r, const vkaa_selector_param_t *restrict param);

struct vkaa_selector_s {
	vkaa_selector_f selector;
};

#endif
