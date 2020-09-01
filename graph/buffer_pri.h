#ifndef _graph_buffer_pri_h_
#define _graph_buffer_pri_h_

#include "buffer.h"
#include <vulkan/vulkan.h>
#include "device_pri.h"

struct graph_memory_heap_s {
	mlog_s *ml;
	graph_dev_s *dev;
	graph_allocator_s *ga;
	VkPhysicalDeviceMemoryProperties properties;
};

struct graph_buffer_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkBuffer buffer;
	VkDeviceMemory memory;
	graph_allocator_s *ga;
	graph_memory_heap_s *heap;
	VkMemoryRequirements require;
};

#endif
