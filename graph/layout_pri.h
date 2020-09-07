#ifndef _graph_layout_pri_h_
#define _graph_layout_pri_h_

#include "layout.h"
#include <vulkan/vulkan.h>
#include "device_pri.h"

struct graph_descriptor_set_layout_param_s {
	uint32_t binding_size;
	uint32_t sampler_size;
	uint32_t binding_number;
	uint32_t sampler_number;
	struct graph_sampler_s **graph_sampler;
	VkDescriptorSetLayoutCreateInfo info;
	VkDescriptorSetLayoutBinding *binding;
	VkSampler *sampler;
};

struct graph_descriptor_set_layout_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkDescriptorSetLayout set_layout;
	graph_allocator_s *ga;
};

struct graph_pipe_layout_param_s {
	uint32_t set_layout_size;
	uint32_t push_constant_range_size;
	uint32_t set_layout_number;
	uint32_t push_constant_range_number;
	graph_descriptor_set_layout_s **rsl;
	VkDescriptorSetLayout *set_layouts;
	VkPushConstantRange *push_constant_ranges;
};

struct graph_pipe_layout_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkPipelineLayout layout;
	graph_allocator_s *ga;
};

struct graph_descriptor_pool_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkDescriptorPool pool;
	graph_allocator_s *ga;
};

struct graph_descriptor_sets_s {
	graph_descriptor_pool_s *pool;
	uint32_t number;
	uint32_t res;
	graph_descriptor_set_layout_s **layout;
	VkDescriptorSet *set;
};

#endif
