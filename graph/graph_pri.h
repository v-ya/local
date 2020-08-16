#ifndef _graph_pri_h_
#define _graph_pri_h_

#include "graph.h"
#include <vulkan/vulkan.h>
#include "allocator_pri.h"
#include "vulkan_debug.h"

struct graph_s {
	mlog_s *ml;
	graph_vulkan_debug_utils_messenger_s *debug;
	VkInstance instance;
	graph_allocator_s *ga;
};

#endif
