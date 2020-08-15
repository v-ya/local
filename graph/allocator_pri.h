#ifndef _graph_allocator_pri_h_
#define _graph_allocator_pri_h_

#include "allocator.h"
#include <vulkan/vulkan.h>

struct graph_allocator_s {
	VkAllocationCallbacks alloc;
};

#endif
