#ifndef _vkaa_var_h_
#define _vkaa_var_h_

#include "vkaa.h"
#include <hashmap.h>

struct vkaa_var_s {
	uintptr_t type_id;
	const vkaa_type_s *type;
	hashmap_t *selector;
	uintptr_t flags;
};

typedef enum vkaa_var_flag_t {
	vkaa_var_flag_const  = 0x00000001,
} vkaa_var_flag_t;

#endif
