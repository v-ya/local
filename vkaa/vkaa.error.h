#ifndef _vkaa_error_h_
#define _vkaa_error_h_

#include "vkaa.h"
#include <hashmap.h>

struct vkaa_error_s {
	hashmap_t n2i;
	hashmap_t i2n;
};

vkaa_error_s* vkaa_error_alloc(void);
vkaa_error_s* vkaa_error_add_error(vkaa_error_s *restrict error, const char *restrict name);
uintptr_t vkaa_error_get_id(vkaa_error_s *restrict error, const char *restrict name);
const char* vkaa_error_get_name(vkaa_error_s *restrict error, uintptr_t id);

#endif
