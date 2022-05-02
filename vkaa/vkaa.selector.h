#ifndef _vkaa_selector_h_
#define _vkaa_selector_h_

#include "vkaa.h"

typedef struct vkaa_selector_param_t {
	const char *name;
	vkaa_var_s *this;
	vkaa_var_s *const *input_list;
	uintptr_t input_number;
} vkaa_selector_param_t;

typedef struct vkaa_selector_rdata_t {
	vkaa_function_f function;
	uintptr_t output_typeid;
	vkaa_var_s *output_must;
} vkaa_selector_rdata_t;

typedef vkaa_selector_rdata_t* (*vkaa_selector_f)(const vkaa_selector_s *restrict r, vkaa_selector_rdata_t *restrict rdata, const vkaa_selector_param_t *restrict param);

struct vkaa_selector_s {
	vkaa_selector_f selector;
};

#endif
