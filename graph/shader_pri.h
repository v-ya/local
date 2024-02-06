#ifndef _graph_shader_pri_h_
#define _graph_shader_pri_h_

#include "shader.h"
#include <vulkan/vulkan.h>
#include "device_pri.h"

typedef struct graph_pipe_shader_stage_t graph_pipe_shader_stage_t;

struct graph_shader_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkShaderModule shader;
	graph_allocator_s *ga;
};

struct graph_shader_spec_s {
	VkSpecializationInfo spec;
};

struct graph_vertex_input_description_s {
	uint32_t bind_size;
	uint32_t attr_size;
	uint32_t bind_number;
	uint32_t attr_number;
	VkVertexInputBindingDescription *bind;
	VkVertexInputAttributeDescription *attr;
};

struct graph_viewports_scissors_s {
	uint32_t viewport_size;
	uint32_t scissor_size;
	uint32_t viewport_number;
	uint32_t scissor_number;
	VkViewport *viewports;
	VkRect2D *scissors;
};

struct graph_pipe_color_blend_s {
	uint32_t at_size;
	uint32_t at_number;
	VkPipelineColorBlendStateCreateInfo info;
	VkPipelineColorBlendAttachmentState attachment[];
};

struct graph_render_pass_param_s {
	uint32_t at_size;
	uint32_t sp_size;
	uint32_t dp_size;
	uint32_t ar_size;
	VkAttachmentDescription *attachment;
	VkSubpassDescription *subpass;
	VkSubpassDependency *dependency;
	VkAttachmentReference *aref;
	VkRenderPassCreateInfo info;
};

struct graph_render_pass_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkRenderPass render;
	graph_allocator_s *ga;
};

struct graph_pipe_cache_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkPipelineCache pipe_cache;
	graph_allocator_s *ga;
};

struct graph_pipe_s {
	mlog_s *ml;
	graph_dev_s *dev;
	graph_pipe_cache_s *cache;
	VkPipeline pipe;
	graph_allocator_s *ga;
};

struct graph_pipe_shader_stage_t {
	refer_string_t name;
	graph_shader_s *gs;
	graph_shader_spec_s *spec;
};

// gpipe param

struct graph_gpipe_param_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkGraphicsPipelineCreateInfo *pi;
	uint32_t shader_number;
	uint32_t res;
	graph_pipe_shader_stage_t *shader_stages;
	VkPipelineShaderStageCreateInfo *shaders;
	graph_vertex_input_description_s *vertex;
	graph_viewports_scissors_s *viewports_scissors;
	graph_pipe_color_blend_s *color_blend;
	struct graph_pipe_layout_s *pipe_layout;
	graph_render_pass_s *render;
	VkGraphicsPipelineCreateInfo info;
	VkPipelineVertexInputStateCreateInfo vertex_input;
	VkPipelineInputAssemblyStateCreateInfo input_assembly;
	VkPipelineTessellationStateCreateInfo tessellation;
	VkPipelineViewportStateCreateInfo viewport;
	VkPipelineRasterizationStateCreateInfo rasterization;
	VkPipelineMultisampleStateCreateInfo multisample;
	VkPipelineDepthStencilStateCreateInfo depth_stencil;
	VkPipelineDynamicStateCreateInfo dynamic;
};

// cpipe param

struct graph_cpipe_param_s {
	mlog_s *ml;
	graph_dev_s *dev;
	VkComputePipelineCreateInfo *pi;
	graph_pipe_shader_stage_t shader_stage;
	struct graph_pipe_layout_s *pipe_layout;
	VkComputePipelineCreateInfo info;
};

// inner func

void graph_pipe_free_func(graph_pipe_s *restrict r);

graph_pipe_shader_stage_t* graph_pipe_shader_stage_initial(graph_pipe_shader_stage_t *restrict stage, const graph_shader_s *restrict shader, const graph_shader_spec_s *restrict spec, const char *restrict entry);
void graph_pipe_shader_stage_finally(graph_pipe_shader_stage_t *restrict stage);

#endif
