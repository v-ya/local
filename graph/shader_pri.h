#ifndef _graph_shader_pri_h_
#define _graph_shader_pri_h_

#include "shader.h"
#include <vulkan/vulkan.h>
#include "device_pri.h"

struct graph_shader_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkShaderModule shader;
	graph_allocator_s *ga;
};

struct graph_shader_spec_s {
	VkSpecializationInfo spec;
};

struct graph_viewports_scissors_s {
	uint32_t viewport_size;
	uint32_t viewport_number;
	uint32_t scissor_size;
	uint32_t scissor_number;
	VkViewport *viewports;
	VkRect2D *scissors;
};

typedef struct graph_shader_stage_t {
	char *name;
	graph_shader_s *gs;
	graph_shader_spec_s *spec;
} graph_shader_stage_t;

struct graph_pipe_layout_param_s {
	;
};

struct graph_pipe_layout_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkPipelineLayout layout;
	graph_allocator_s *ga;
};

struct graph_pipe_s {
	;
};

struct graph_gpipe_param_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkGraphicsPipelineCreateInfo *pi;
	uint32_t shader_number;
	uint32_t res;
	graph_shader_stage_t *shader_stages;
	VkPipelineShaderStageCreateInfo *shaders;
	graph_viewports_scissors_s *viewports_scissors;
	graph_pipe_layout_s *pipe_layout;
	VkGraphicsPipelineCreateInfo info;
	VkPipelineVertexInputStateCreateInfo vertex_input;
	VkPipelineInputAssemblyStateCreateInfo input_assembly;
	VkPipelineTessellationStateCreateInfo tessellation;
	VkPipelineViewportStateCreateInfo viewport;
	VkPipelineRasterizationStateCreateInfo rasterization;
	VkPipelineMultisampleStateCreateInfo multisample;
	VkPipelineDepthStencilStateCreateInfo depth_stencil;
	VkPipelineColorBlendStateCreateInfo color_blend;
	VkPipelineDynamicStateCreateInfo dynamic;
};

#endif
