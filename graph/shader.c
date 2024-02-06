#include "shader_pri.h"
#include "type_pri.h"
#include "system.h"

static void graph_shader_free_func(graph_shader_s *restrict r)
{
	void *v;
	if ((v = r->shader)) vkDestroyShaderModule(r->dev->dev, (VkShaderModule) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_shader_s* graph_shader_alloc(struct graph_dev_s *restrict dev, const void *restrict code, size_t size)
{
	graph_shader_s *restrict r;
	void *need_free;
	VkShaderModuleCreateInfo info;
	VkResult ret;
	r = NULL;
	need_free = NULL;
	if (!size)
	{
		code = need_free = graph_system_load_file((const char *) code, &size);
		if (!code) goto label_miss;
	}
	r = (graph_shader_s *) refer_alloz(sizeof(graph_shader_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_shader_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		info.pNext = NULL;
		info.flags = 0;
		info.codeSize = size;
		info.pCode = code;
		ret = vkCreateShaderModule(dev->dev, &info, &r->ga->alloc, &r->shader);
		if (ret) goto label_fail;
	}
	label_return:
	if (need_free) free(need_free);
	label_miss:
	return r;
	label_fail:
	mlog_printf(r->ml, "[graph_shader_alloc] vkCreateShaderModule = %d\n", ret);
	refer_free(r);
	r = NULL;
	goto label_return;
}

graph_vertex_input_description_s* graph_vertex_input_description_alloc(uint32_t bind_number, uint32_t attr_number)
{
	graph_vertex_input_description_s *restrict r;
	r = (graph_vertex_input_description_s *) refer_alloz(sizeof(graph_vertex_input_description_s) +
		sizeof(VkVertexInputBindingDescription) * bind_number +
		sizeof(VkVertexInputAttributeDescription) * attr_number);
	if (r)
	{
		r->bind_size = bind_number;
		r->attr_size = attr_number;
		r->bind = (VkVertexInputBindingDescription *) (r + 1);
		r->attr = (VkVertexInputAttributeDescription *) (r->bind + bind_number);
	}
	return r;
}

graph_vertex_input_description_s* graph_vertex_input_description_set_bind(graph_vertex_input_description_s *restrict desc, uint32_t index, uint32_t binding, uint32_t stride, graph_vertex_input_rate_t irate)
{
	if (index < desc->bind_size)
	{
		VkVertexInputBindingDescription *restrict p;
		p = desc->bind + index;
		p->binding = binding;
		p->stride = stride;
		p->inputRate = graph_vertex_input_rate2vk(irate);
		if (index >= desc->bind_number)
			desc->bind_number = index + 1;
		return desc;
	}
	return NULL;
}

graph_vertex_input_description_s* graph_vertex_input_description_set_attr(graph_vertex_input_description_s *restrict desc, uint32_t index, uint32_t location, uint32_t binding, uint32_t offset, graph_format_t format)
{
	if (index < desc->attr_size)
	{
		VkVertexInputAttributeDescription *restrict p;
		p = desc->attr + index;
		p->location = location;
		p->binding = binding;
		p->format = graph_format2vk(format);
		p->offset = offset;
		if (index >= desc->attr_number)
			desc->attr_number = index + 1;
		return desc;
	}
	return NULL;
}

graph_viewports_scissors_s* graph_viewports_scissors_alloc(uint32_t viewport_number, uint32_t scissor_number)
{
	graph_viewports_scissors_s *restrict r;
	r = (graph_viewports_scissors_s *) refer_alloc(
		sizeof(graph_viewports_scissors_s) +
		sizeof(VkViewport) * viewport_number +
		sizeof(VkRect2D) * scissor_number);
	if (r)
	{
		r->viewport_size = viewport_number;
		r->scissor_size = scissor_number;
		r->viewport_number = 0;
		r->scissor_number = 0;
		r->viewports = (VkViewport *) (r + 1);
		r->scissors = (VkRect2D *) (r->viewports + viewport_number);
		return r;
	}
	return NULL;
}

graph_viewports_scissors_s* graph_viewports_scissors_append_viewport(graph_viewports_scissors_s *restrict r, float x, float y, float w, float h, float dmin, float dmax)
{
	VkViewport *restrict p;
	if (r->viewport_number < r->viewport_size)
	{
		p = r->viewports + r->viewport_number++;
		p->x = x;
		p->y = y;
		p->width = w;
		p->height = h;
		p->minDepth = dmin;
		p->maxDepth = dmax;
		return r;
	}
	return NULL;
}

graph_viewports_scissors_s* graph_viewports_scissors_append_scissor(graph_viewports_scissors_s *restrict r, int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	VkRect2D *restrict p;
	if (r->scissor_number < r->scissor_size)
	{
		p = r->scissors + r->scissor_number++;
		p->offset.x = x;
		p->offset.y = y;
		p->extent.width = w;
		p->extent.height = h;
		return r;
	}
	return NULL;
}

graph_viewports_scissors_s* graph_viewports_scissors_update_viewport(graph_viewports_scissors_s *restrict r, uint32_t index, float x, float y, float w, float h, float dmin, float dmax)
{
	VkViewport *restrict p;
	if (index < r->viewport_number)
	{
		p = r->viewports + index;
		p->x = x;
		p->y = y;
		p->width = w;
		p->height = h;
		p->minDepth = dmin;
		p->maxDepth = dmax;
		return r;
	}
	return NULL;
}

graph_viewports_scissors_s* graph_viewports_scissors_update_scissor(graph_viewports_scissors_s *restrict r, uint32_t index, int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	VkRect2D *restrict p;
	if (index < r->scissor_number)
	{
		p = r->scissors + index;
		p->offset.x = x;
		p->offset.y = y;
		p->extent.width = w;
		p->extent.height = h;
		return r;
	}
	return NULL;
}

graph_pipe_color_blend_s* graph_pipe_color_blend_alloc(uint32_t attachment_number)
{
	graph_pipe_color_blend_s *restrict r;
	if (attachment_number)
	{
		r = (graph_pipe_color_blend_s *) refer_alloz(sizeof(graph_pipe_color_blend_s) + sizeof(VkPipelineColorBlendAttachmentState) * attachment_number);
		if (r)
		{
			r->at_size = attachment_number;
			r->info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			r->info.pAttachments = r->attachment;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

void graph_pipe_color_blend_set_logic(graph_pipe_color_blend_s *restrict r, graph_bool_t logic_op, graph_logic_op_t op)
{
	r->info.logicOpEnable = !!logic_op;
	r->info.logicOp = graph_logic_op2vk(op);
}

void graph_pipe_color_blend_set_constants(graph_pipe_color_blend_s *restrict r, const float constants[4])
{
	r->info.blendConstants[0] = constants[0];
	r->info.blendConstants[1] = constants[1];
	r->info.blendConstants[2] = constants[2];
	r->info.blendConstants[3] = constants[3];
}

graph_pipe_color_blend_s* graph_pipe_color_blend_append_attachment(
	graph_pipe_color_blend_s *restrict r, graph_bool_t enable, graph_color_component_mask_t mask,
	graph_blend_factor_t s_color, graph_blend_factor_t d_color, graph_blend_op_t op_color,
	graph_blend_factor_t s_alpha, graph_blend_factor_t d_alpha, graph_blend_op_t op_alpha)
{
	if (r->at_number < r->at_size)
	{
		VkPipelineColorBlendAttachmentState *restrict p;
		p = r->attachment + r->at_number;
		p->blendEnable = !!enable;
		p->srcColorBlendFactor = graph_blend_factor2vk(s_color);
		p->dstColorBlendFactor = graph_blend_factor2vk(d_color);
		p->colorBlendOp = graph_blend_op2vk(op_color);
		p->srcAlphaBlendFactor = graph_blend_factor2vk(s_alpha);
		p->dstAlphaBlendFactor = graph_blend_factor2vk(d_alpha);
		p->alphaBlendOp = graph_blend_op2vk(op_alpha);
		p->colorWriteMask = (VkColorComponentFlags) mask;
		r->info.attachmentCount = ++r->at_number;
		return r;
	}
	return NULL;
}

graph_render_pass_param_s* graph_render_pass_param_alloc(uint32_t attachment_number, uint32_t subpass_number, uint32_t dependency_number, uint32_t aref_number)
{
	graph_render_pass_param_s *restrict r;
	r = (graph_render_pass_param_s *) refer_alloz(
		sizeof(graph_render_pass_param_s) +
		sizeof(VkAttachmentDescription) * attachment_number +
		sizeof(VkSubpassDescription) * subpass_number +
		sizeof(VkSubpassDependency) * dependency_number +
		sizeof(VkAttachmentReference) * aref_number
	);
	if (r)
	{
		r->at_size = attachment_number;
		r->sp_size = subpass_number;
		r->dp_size = dependency_number;
		r->ar_size = aref_number;
		r->attachment = (VkAttachmentDescription *) (r + 1);
		r->subpass = (VkSubpassDescription *) (r->attachment + attachment_number);
		r->dependency = (VkSubpassDependency *) (r->subpass + subpass_number);
		r->aref = (VkAttachmentReference *) (r->dependency + dependency_number);
		r->info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		r->info.pAttachments = r->attachment;
		r->info.pSubpasses = r->subpass;
		r->info.pDependencies = r->dependency;
	}
	return r;
}

graph_render_pass_param_s* graph_render_pass_param_set_attachment(
	graph_render_pass_param_s *restrict r, uint32_t index,
	graph_format_t format, graph_sample_count_t sample,
	graph_attachment_load_op_t load, graph_attachment_store_op_t store,
	graph_attachment_load_op_t stencil_load, graph_attachment_store_op_t stencil_store,
	graph_image_layout_t initial, graph_image_layout_t final)
{
	VkAttachmentDescription *restrict p;
	if (index < r->at_size)
	{
		p = r->attachment + index;
		p->flags = 0;
		p->format = graph_format2vk(format);
		p->samples = (VkSampleCountFlagBits) sample;
		p->loadOp = graph_attachment_load_op2vk(load);
		p->storeOp = graph_attachment_store_op2vk(store);
		p->stencilLoadOp = graph_attachment_load_op2vk(stencil_load);
		p->stencilStoreOp = graph_attachment_store_op2vk(stencil_store);
		p->initialLayout = graph_image_layout2vk(initial);
		p->finalLayout = graph_image_layout2vk(final);
		if (index >= r->info.attachmentCount)
			r->info.attachmentCount = index + 1;
		return r;
	}
	return NULL;
}

graph_render_pass_param_s* graph_render_pass_param_set_subpass(graph_render_pass_param_s *restrict r, uint32_t index, graph_pipeline_bind_point_t type)
{
	VkSubpassDescription *restrict p;
	if (index < r->sp_size && (uint32_t) type < graph_pipeline_bind_point$number)
	{
		p = r->subpass + index;
		p->pipelineBindPoint = graph_pipeline_bind_point2vk(type);
		if (index >= r->info.subpassCount)
			r->info.subpassCount = index + 1;
		return r;
	}
	return NULL;
}

graph_render_pass_param_s* graph_render_pass_param_set_subpass_color(graph_render_pass_param_s *restrict r, uint32_t index, uint32_t n, uint32_t at_index[], graph_image_layout_t layout[])
{
	VkSubpassDescription *restrict p;
	if (index < r->sp_size && n <= r->ar_size)
	{
		VkAttachmentReference *pr;
		uint32_t i;
		p = r->subpass + index;
		pr = r->aref;
		for (i = 0; i < n; ++i)
		{
			if (at_index[i] >= r->info.attachmentCount)
				goto label_fail;
			pr[i].attachment = at_index[i];
			pr[i].layout = graph_image_layout2vk(layout[i]);
		}
		p->colorAttachmentCount = n;
		p->pColorAttachments = r->aref;
		r->ar_size -= n;
		r->aref += n;
		return r;
	}
	label_fail:
	return NULL;
}

static void graph_render_pass_free_func(graph_render_pass_s *restrict r)
{
	void *v;
	if ((v = r->render)) vkDestroyRenderPass(r->dev->dev, (VkRenderPass) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_render_pass_s* graph_render_pass_alloc(graph_render_pass_param_s *restrict param, struct graph_dev_s *restrict dev)
{
	graph_render_pass_s *restrict r;
	VkResult ret;
	r = (graph_render_pass_s *) refer_alloz(sizeof(graph_render_pass_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_render_pass_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		ret = vkCreateRenderPass(dev->dev, &param->info, &r->ga->alloc, &r->render);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_render_pass_alloc] vkCreateRenderPass = %d\n", ret);
		refer_free(r);
	}
	return NULL;
}

static void graph_pipe_cache_free_func(graph_pipe_cache_s *restrict r)
{
	void *v;
	if ((v = r->pipe_cache)) vkDestroyPipelineCache(r->dev->dev, (VkPipelineCache) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_pipe_cache_s* graph_pipe_cache_alloc(struct graph_dev_s *restrict dev)
{
	graph_pipe_cache_s *restrict r;
	VkPipelineCacheCreateInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.initialDataSize = 0;
	info.pInitialData = NULL;
	r = (graph_pipe_cache_s *) refer_alloz(sizeof(graph_pipe_cache_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_pipe_cache_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		ret = vkCreatePipelineCache(dev->dev, &info, &r->ga->alloc, &r->pipe_cache);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_pipe_cache_alloc] vkCreatePipelineCache = %d\n", ret);
		refer_free(r);
	}
	return NULL;
}

// inner

void graph_pipe_free_func(graph_pipe_s *restrict r)
{
	void *v;
	if ((v = r->pipe)) vkDestroyPipeline(r->dev->dev, (VkPipeline) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->cache)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_pipe_shader_stage_t* graph_pipe_shader_stage_initial(graph_pipe_shader_stage_t *restrict stage, const graph_shader_s *restrict shader, const graph_shader_spec_s *restrict spec, const char *restrict entry)
{
	if (shader && entry && (stage->name = refer_dump_string(entry)))
	{
		stage->gs = (graph_shader_s *) refer_save(shader);
		stage->spec = (graph_shader_spec_s *) refer_save(spec);
		return stage;
	}
	stage->name = NULL;
	stage->gs = NULL;
	stage->spec = NULL;
	return NULL;
}

void graph_pipe_shader_stage_finally(graph_pipe_shader_stage_t *restrict stage)
{
	if (stage->name) refer_free(stage->name);
	if (stage->gs) refer_free(stage->gs);
	if (stage->spec) refer_free(stage->spec);
	stage->name = NULL;
	stage->gs = NULL;
	stage->spec = NULL;
}
