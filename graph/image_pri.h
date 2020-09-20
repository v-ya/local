#ifndef _graph_image_pri_h_
#define _graph_image_pri_h_

#include "image.h"
#include <vulkan/vulkan.h>
#include "graph_pri.h"
#include "surface_pri.h"
#include "shader_pri.h"

struct graph_image_param_s {
	VkImageCreateInfo info;
};

struct graph_image_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkImage image;
	VkDeviceMemory memory;
	graph_allocator_s *ga;
	struct graph_memory_heap_s *heap;
	VkMemoryRequirements require;
	VkImageType type;
	VkExtent3D extent;
};

struct graph_image_view_param_s {
	VkImageViewCreateFlags flags;
	VkImageViewType type;
	VkFormat format;
	VkComponentMapping components;
	VkImageSubresourceRange range;
};

struct graph_image_view_s {
	mlog_s *ml;
	refer_t depend;
	graph_dev_s *dev;
	VkImage image;
	VkImageView view;
	graph_allocator_s *ga;
};

struct graph_frame_buffer_s {
	mlog_s *ml;
	graph_dev_s *dev;
	graph_render_pass_s *render;
	VkFramebuffer frame_buffer;
	graph_allocator_s *ga;
};

#endif
