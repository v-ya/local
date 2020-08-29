#ifndef _graph_command_pri_h_
#define _graph_command_pri_h_

#include "command.h"
#include <vulkan/vulkan.h>
#include "graph_pri.h"
#include "device_pri.h"

struct graph_command_pool_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkCommandPool cpool;
	graph_allocator_s *ga;
};

#endif
