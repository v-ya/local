#ifndef _vkaa_scope_h_
#define _vkaa_scope_h_

#include "vkaa.h"
#include <hashmap.h>

struct vkaa_scope_s {
	vkaa_scope_s *parent;
	uintptr_t level;
	hashmap_t var;
};

#endif
