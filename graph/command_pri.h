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
	VkClearValue clear[2];
	uint32_t clear_number;
	uint32_t primary_size;
	VkCommandBuffer primary[];
};

struct graph_command_pipe_barrier_param_s {
	uint32_t memory_size;
	uint32_t memory_number;
	uint32_t buffer_size;
	uint32_t buffer_number;
	uint32_t image_size;
	uint32_t image_number;
	VkMemoryBarrier *memory;
	VkBufferMemoryBarrier *buffer;
	VkImageMemoryBarrier *image;
};

struct graph_semaphore_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkSemaphore semaphore;
	graph_allocator_s *ga;
};

struct graph_fence_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkFence fence;
	graph_allocator_s *ga;
};

#endif
