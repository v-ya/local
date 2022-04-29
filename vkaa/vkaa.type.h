#ifndef _vkaa_type_h_
#define _vkaa_type_h_

#include "vkaa.h"
#include <vattr.h>

struct vkaa_type_s {
	uintptr_t id;
	const char *name;
	vattr_s *selector;
};

#endif
