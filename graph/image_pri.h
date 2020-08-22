#ifndef _graph_image_pri_h_
#define _graph_image_pri_h_

#include "image.h"
#include <vulkan/vulkan.h>
#include "graph_pri.h"
#include "surface_pri.h"

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

#endif
