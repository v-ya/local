#include "shader_pri.h"
#include "layout_pri.h"
#include "type_pri.h"
#include "system.h"

static void graph_shader_free_func(register graph_shader_s *restrict r)
{
	register void *v;
	if ((v = r->shader)) vkDestroyShaderModule(r->dev->dev, (VkShaderModule) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_shader_s* graph_shader_alloc(register struct graph_dev_s *restrict dev, const void *restrict code, size_t size)
{
	register graph_shader_s *restrict r;
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
	register graph_vertex_input_description_s *restrict r;
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

void graph_vertex_input_description_set_bind(register graph_vertex_input_description_s *restrict desc, uint32_t index, uint32_t binding, uint32_t stride, graph_vertex_input_rate_t irate)
{
	if (index < desc->bind_size)
	{
		register VkVertexInputBindingDescription *restrict p;
		p = desc->bind + index;
		p->binding = binding;
		p->stride = stride;
		p->inputRate = graph_vertex_input_rate2vk(irate);
		if (index >= desc->bind_number)
			desc->bind_number = index + 1;
	}
}

void graph_vertex_input_description_set_attr(register graph_vertex_input_description_s *restrict desc, uint32_t index, uint32_t location, uint32_t binding, uint32_t offset, graph_format_t format)
{
	if (index < desc->attr_size)
	{
		register VkVertexInputAttributeDescription *restrict p;
		p = desc->attr + index;
		p->location = location;
		p->binding = binding;
		p->format = graph_format2vk(format);
		p->offset = offset;
		if (index >= desc->attr_number)
			desc->attr_number = index + 1;
	}
}

graph_viewports_scissors_s* graph_viewports_scissors_alloc(uint32_t viewport_number, uint32_t scissor_number)
{
	register graph_viewports_scissors_s *restrict r;
	if (viewport_number && scissor_number)
	{
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
	}
	return NULL;
}

graph_viewports_scissors_s* graph_viewports_scissors_append_viewport(register graph_viewports_scissors_s *restrict r, float x, float y, float w, float h, float dmin, float dmax)
{
	register VkViewport *restrict p;
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

graph_viewports_scissors_s* graph_viewports_scissors_append_scissor(register graph_viewports_scissors_s *restrict r, int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	register VkRect2D *restrict p;
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

graph_viewports_scissors_s* graph_viewports_scissors_update_viewport(register graph_viewports_scissors_s *restrict r, uint32_t index, float x, float y, float w, float h, float dmin, float dmax)
{
	register VkViewport *restrict p;
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

graph_viewports_scissors_s* graph_viewports_scissors_update_scissor(register graph_viewports_scissors_s *restrict r, uint32_t index, int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	register VkRect2D *restrict p;
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
	register graph_pipe_color_blend_s *restrict r;
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

void graph_pipe_color_blend_set_logic(register graph_pipe_color_blend_s *restrict r, graph_bool_t logic_op, graph_logic_op_t op)
{
	r->info.logicOpEnable = !!logic_op;
	r->info.logicOp = graph_logic_op2vk(op);
}

void graph_pipe_color_blend_set_constants(register graph_pipe_color_blend_s *restrict r, float constants[4])
{
	r->info.blendConstants[0] = constants[0];
	r->info.blendConstants[1] = constants[1];
	r->info.blendConstants[2] = constants[2];
	r->info.blendConstants[3] = constants[3];
}

graph_pipe_color_blend_s* graph_pipe_color_blend_append_attachment(
	register graph_pipe_color_blend_s *restrict r, graph_bool_t enable, graph_color_component_mask_t mask,
	graph_blend_factor_t s_color, graph_blend_factor_t d_color, graph_blend_op_t op_color,
	graph_blend_factor_t s_alpha, graph_blend_factor_t d_alpha, graph_blend_op_t op_alpha)
{
	if (r->at_number < r->at_size)
	{
		register VkPipelineColorBlendAttachmentState *restrict p;
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
	register graph_render_pass_param_s *restrict r;
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
	register graph_render_pass_param_s *restrict r, uint32_t index,
	graph_format_t format, graph_sample_count_t sample,
	graph_attachment_load_op_t load, graph_attachment_store_op_t store,
	graph_attachment_load_op_t stencil_load, graph_attachment_store_op_t stencil_store,
	graph_image_layout_t initial, graph_image_layout_t final)
{
	register VkAttachmentDescription *restrict p;
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

graph_render_pass_param_s* graph_render_pass_param_set_subpass(register graph_render_pass_param_s *restrict r, uint32_t index, graph_pipeline_bind_point_t type)
{
	register VkSubpassDescription *restrict p;
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

graph_render_pass_param_s* graph_render_pass_param_set_subpass_color(register graph_render_pass_param_s *restrict r, uint32_t index, uint32_t n, uint32_t at_index[], graph_image_layout_t layout[])
{
	register VkSubpassDescription *restrict p;
	if (index < r->sp_size && n <= r->ar_size)
	{
		register VkAttachmentReference *pr;
		register uint32_t i;
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

static void graph_render_pass_free_func(register graph_render_pass_s *restrict r)
{
	register void *v;
	if ((v = r->render)) vkDestroyRenderPass(r->dev->dev, (VkRenderPass) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_render_pass_s* graph_render_pass_alloc(register graph_render_pass_param_s *restrict param, register struct graph_dev_s *restrict dev)
{
	register graph_render_pass_s *restrict r;
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

static void graph_pipe_cache_free_func(register graph_pipe_cache_s *restrict r)
{
	register void *v;
	if ((v = r->pipe_cache)) vkDestroyPipelineCache(r->dev->dev, (VkPipelineCache) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_pipe_cache_s* graph_pipe_cache_alloc(register struct graph_dev_s *restrict dev)
{
	register graph_pipe_cache_s *restrict r;
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

static void graph_pipe_free_func(register graph_pipe_s *restrict r)
{
	register void *v;
	if ((v = r->pipe)) vkDestroyPipeline(r->dev->dev, (VkPipeline) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->cache)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_pipe_s* graph_pipe_alloc_graphics(register const graph_gpipe_param_s *restrict param, register const graph_pipe_cache_s *restrict cache)
{
	register graph_pipe_s *restrict r;
	VkResult ret;
	if (param->pi)
	{
		r = (graph_pipe_s *) refer_alloz(sizeof(graph_pipe_s));
		if (r)
		{
			refer_set_free(r, (refer_free_f) graph_pipe_free_func);
			r->ml = (mlog_s *) refer_save(param->ml);
			r->dev = (graph_dev_s *) refer_save(param->dev);
			r->cache = (graph_pipe_cache_s *) refer_save(cache);
			r->ga = (graph_allocator_s *) refer_save(param->dev->ga);
			ret = vkCreateGraphicsPipelines(r->dev->dev, cache?cache->pipe_cache:NULL, 1, param->pi, &r->ga->alloc, &r->pipe);
			if (!ret) return r;
			mlog_printf(r->ml, "[graph_pipe_alloc_graphics] vkCreateGraphicsPipelines = %d\n", ret);
			refer_free(r);
		}
	}
	else mlog_printf(param->ml, "[graph_pipe_alloc_graphics] make sure call 'graph_gpipe_param_ok' ok\n");
	return NULL;
}

static void graph_gpipe_param_free_func(register graph_gpipe_param_s *restrict r)
{
	register void *v;
	register graph_pipe_shader_stage_t *p;
	register uint32_t n;
	if ((v = (void *) r->dynamic.pDynamicStates)) free(v);
	if ((v = r->vertex)) refer_free(v);
	if ((v = r->viewports_scissors)) refer_free(v);
	if ((v = r->color_blend)) refer_free(v);
	if ((v = r->pipe_layout)) refer_free(v);
	if ((v = r->render)) refer_free(v);
	p = r->shader_stages;
	n = r->shader_number;
	do {
		if ((v = p->name)) free(v);
		if ((v = p->gs)) refer_free(v);
		if ((v = p->spec)) refer_free(v);
		++p;
	} while (--n);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
}

graph_gpipe_param_s* graph_gpipe_param_alloc(register struct graph_dev_s *restrict dev, uint32_t shader_number)
{
	register graph_gpipe_param_s *restrict r;
	if (shader_number)
	{
		r = (graph_gpipe_param_s *) refer_alloz(sizeof(graph_gpipe_param_s) +
			(sizeof(graph_pipe_shader_stage_t) + sizeof(VkPipelineShaderStageCreateInfo)) * shader_number);
		if (r)
		{
			refer_set_free(r, (refer_free_f) graph_gpipe_param_free_func);
			r->ml = (mlog_s *) refer_save(dev->ml);
			r->dev = (graph_dev_s *) refer_save(dev);
			r->shader_number = shader_number;
			r->shader_stages = (graph_pipe_shader_stage_t *) (r + 1);
			r->shaders = (VkPipelineShaderStageCreateInfo *) (r->shader_stages + shader_number);
			r->info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			r->info.pStages = r->shaders;
			r->info.pRasterizationState = &r->rasterization;
			r->info.pMultisampleState = &r->multisample;
			r->vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			r->input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			r->tessellation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			r->viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			r->rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			r->multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			r->depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			r->dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			r->rasterization.lineWidth = 1.0f;
			r->multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			r->multisample.minSampleShading = 1.0f;
			do {
				r->shaders[--shader_number].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			} while (shader_number);
			return r;
		}
	}
	return NULL;
}

graph_gpipe_param_s* graph_gpipe_param_append_shader(register graph_gpipe_param_s *restrict r, const graph_shader_s *restrict shader, graph_shader_type_t type, const char *restrict entry, const graph_shader_spec_s *restrict spec)
{
	register VkPipelineShaderStageCreateInfo *restrict p;
	register graph_pipe_shader_stage_t *restrict s;
	uint32_t i;
	r->pi = NULL;
	if (!shader || !entry) goto label_arg;
	if ((i = r->info.stageCount) >= r->shader_number) goto label_shader_max;
	p = r->shaders + i;
	s = r->shader_stages + i;
	if ((s->name = graph_system_dump_string(entry)))
	{
		s->gs = (graph_shader_s *) refer_save(shader);
		s->spec = (graph_shader_spec_s *) refer_save(spec);
		p->flags = 0;
		p->stage = (VkShaderStageFlagBits) type;
		p->module = shader->shader;
		p->pName = s->name;
		if (spec) p->pSpecializationInfo = &spec->spec;
		r->info.stageCount = i + 1;
		return r;
	}
	mlog_printf(r->ml, "[graph_gpipe_param_append_shader] malloc fail\n");
	label_return_null:
	return NULL;
	label_arg:
	mlog_printf(r->ml, "[graph_gpipe_param_append_shader] !shader(%p) || !entry(%s)\n", shader, entry);
	goto label_return_null;
	label_shader_max:
	mlog_printf(r->ml, "[graph_gpipe_param_append_shader] shader is maximum[%u]\n", r->shader_number);
	goto label_return_null;
}

void graph_gpipe_param_set_vertex(register graph_gpipe_param_s *restrict r, register const graph_vertex_input_description_s *restrict vertex)
{
	r->pi = NULL;
	if (r->vertex)
	{
		refer_free(r->vertex);
		r->vertex = NULL;
	}
	if (vertex)
	{
		r->vertex = (graph_vertex_input_description_s *) refer_save(vertex);
		r->vertex_input.vertexBindingDescriptionCount = vertex->bind_number;
		r->vertex_input.pVertexBindingDescriptions = vertex->bind;
		r->vertex_input.vertexAttributeDescriptionCount = vertex->attr_number;
		r->vertex_input.pVertexAttributeDescriptions = vertex->attr;
	}
	else
	{
		r->vertex_input.vertexBindingDescriptionCount = 0;
		r->vertex_input.pVertexBindingDescriptions = NULL;
		r->vertex_input.vertexAttributeDescriptionCount = 0;
		r->vertex_input.pVertexAttributeDescriptions = NULL;
	}
	r->info.pVertexInputState = &r->vertex_input;
}

graph_gpipe_param_s* graph_gpipe_param_set_assembly(register graph_gpipe_param_s *restrict r, graph_primitive_topology_t pt, graph_bool_t primitive_restart)
{
	r->pi = NULL;
	r->info.pInputAssemblyState = NULL;
	if ((uint32_t) pt < graph_primitive_topology$number)
	{
		r->input_assembly.flags = 0;
		r->input_assembly.topology = graph_primitive_topology2vk(pt);
		r->input_assembly.primitiveRestartEnable = !!primitive_restart;
		r->info.pInputAssemblyState = &r->input_assembly;
		return r;
	}
	return NULL;
}

graph_gpipe_param_s* graph_gpipe_param_set_viewport(register graph_gpipe_param_s *restrict r, const graph_viewports_scissors_s *restrict gvs)
{
	r->pi = NULL;
	r->info.pViewportState = NULL;
	if (r->viewports_scissors)
	{
		refer_free(r->viewports_scissors);
		r->viewports_scissors = NULL;
	}
	if (gvs && gvs->viewport_number && gvs->scissor_number)
	{
		r->viewports_scissors = (graph_viewports_scissors_s *) refer_save(gvs);
		r->viewport.flags = 0;
		r->viewport.viewportCount = gvs->viewport_number;
		r->viewport.pViewports = gvs->viewports;
		r->viewport.scissorCount = gvs->scissor_number;
		r->viewport.pScissors = gvs->scissors;
		r->info.pViewportState = &r->viewport;
		return r;
	}
	return NULL;
}

graph_gpipe_param_s* graph_gpipe_param_set_color_blend(register graph_gpipe_param_s *restrict r, register const graph_pipe_color_blend_s *restrict blend)
{
	r->pi = NULL;
	r->info.pColorBlendState = NULL;
	if (r->color_blend)
	{
		refer_free(r->color_blend);
		r->color_blend = NULL;
	}
	if (blend)
	{
		r->color_blend = (graph_pipe_color_blend_s *) refer_save(blend);
		r->info.pColorBlendState = &blend->info;
		return r;
	}
	return NULL;
}

void graph_gpipe_param_set_rasterization_depth_clamp(register graph_gpipe_param_s *restrict r, graph_bool_t depth_clamp)
{
	r->pi = NULL;
	r->rasterization.depthClampEnable = !!depth_clamp;
}

void graph_gpipe_param_set_rasterization_discard(register graph_gpipe_param_s *restrict r, graph_bool_t discard)
{
	r->pi = NULL;
	r->rasterization.rasterizerDiscardEnable = !!discard;
}

void graph_gpipe_param_set_rasterization_polygon(register graph_gpipe_param_s *restrict r, graph_polygon_mode_t polygon)
{
	r->pi = NULL;
	if ((uint32_t) polygon < graph_polygon_mode$number)
		r->rasterization.polygonMode = graph_polygon_mode2vk(polygon);
}

void graph_gpipe_param_set_rasterization_cull(register graph_gpipe_param_s *restrict r, graph_cull_mode_flags_t cull)
{
	r->pi = NULL;
	r->rasterization.cullMode = (VkCullModeFlags) cull;
}

void graph_gpipe_param_set_rasterization_front_face(register graph_gpipe_param_s *restrict r, graph_front_face_t front_face)
{
	r->pi = NULL;
	r->rasterization.frontFace = (VkFrontFace) front_face;
}

void graph_gpipe_param_set_rasterization_depth_bias(register graph_gpipe_param_s *restrict r, float constant_factor, float clamp, float slope_factor)
{
	r->pi = NULL;
	r->rasterization.depthBiasEnable = 1;
	r->rasterization.depthBiasConstantFactor = constant_factor;
	r->rasterization.depthBiasClamp = clamp;
	r->rasterization.depthBiasSlopeFactor = slope_factor;
}

void graph_gpipe_param_set_rasterization_line_width(register graph_gpipe_param_s *restrict r, float line_width)
{
	r->pi = NULL;
	r->rasterization.lineWidth = line_width;
}

void graph_gpipe_param_set_multisample_sample_count(register graph_gpipe_param_s *restrict r, graph_sample_count_t sample_count)
{
	r->pi = NULL;
	switch (sample_count)
	{
		case graph_sample_count_1:
		case graph_sample_count_2:
		case graph_sample_count_4:
		case graph_sample_count_8:
		case graph_sample_count_16:
		case graph_sample_count_32:
		case graph_sample_count_64:
			r->multisample.rasterizationSamples = (VkPipelineMultisampleStateCreateFlags) sample_count;
			break;
	}
}

void graph_gpipe_param_set_multisample_sample_shading(register graph_gpipe_param_s *restrict r, graph_bool_t sample_shading)
{
	r->pi = NULL;
	r->multisample.sampleShadingEnable = !!sample_shading;
}

void graph_gpipe_param_set_multisample_min_sample_shading(register graph_gpipe_param_s *restrict r, float min_sample_shading)
{
	r->pi = NULL;
	r->multisample.minSampleShading = min_sample_shading;
}

void graph_gpipe_param_set_multisample_alpha2coverage(register graph_gpipe_param_s *restrict r, graph_bool_t alpha2coverage)
{
	r->pi = NULL;
	r->multisample.alphaToCoverageEnable = !!alpha2coverage;
}

void graph_gpipe_param_set_multisample_alpha2one(register graph_gpipe_param_s *restrict r, graph_bool_t alpha2one)
{
	r->pi = NULL;
	r->multisample.alphaToOneEnable = !!alpha2one;
}

graph_gpipe_param_s* graph_gpipe_param_set_dynamic(register graph_gpipe_param_s *restrict r, uint32_t n, const graph_dynamic_t dynamic[])
{
	VkDynamicState *restrict p;
	uint32_t i;
	r->pi = NULL;
	r->info.pDynamicState = NULL;
	r->dynamic.dynamicStateCount = 0;
	if ((p = (VkDynamicState *) r->dynamic.pDynamicStates))
	{
		r->dynamic.pDynamicStates = NULL;
		free(p);
	}
	if (n)
	{
		p = (VkDynamicState *) malloc(sizeof(VkDynamicState) * n);
		if (!p) goto label_fail;
		for (i = 0; i < n; ++i)
		{
			if ((uint32_t) dynamic[i] >= graph_dynamic$number)
				goto label_fail;
			p[i] = graph_dynamic2vk(dynamic[i]);
		}
		r->dynamic.pDynamicStates = p;
		r->dynamic.dynamicStateCount = n;
	}
	r->info.pDynamicState = &r->dynamic;
	return r;
	label_fail:
	if (p) free(p);
	return NULL;
}

void graph_gpipe_param_set_layout(register graph_gpipe_param_s *restrict r, register const struct graph_pipe_layout_s *restrict layout)
{
	r->pi = NULL;
	r->info.layout = NULL;
	if (r->pipe_layout)
	{
		refer_free(r->pipe_layout);
		r->pipe_layout = NULL;
	}
	if (layout)
	{
		r->pipe_layout = (graph_pipe_layout_s *) refer_save(layout);
		r->info.layout = layout->layout;
	}
}

void graph_gpipe_param_set_render(register graph_gpipe_param_s *restrict r, register const graph_render_pass_s *restrict render, uint32_t subpass_index)
{
	r->pi = NULL;
	r->info.renderPass = NULL;
	r->info.subpass = 0;
	if (r->render)
	{
		refer_free(r->render);
		r->render = NULL;
	}
	if (render)
	{
		r->render = (graph_render_pass_s *) refer_save(render);
		r->info.renderPass = render->render;
		r->info.subpass = subpass_index;
	}
}

graph_gpipe_param_s* graph_gpipe_param_ok(register graph_gpipe_param_s *restrict r)
{
	static char empty[] = {0};
	r->pi = NULL;
	// pViewportState allow null, while used dynamic viewport. maybe.
	if (r->info.stageCount && r->info.pVertexInputState && r->info.pInputAssemblyState &&
		r->info.pColorBlendState && r->info.pDynamicState && r->info.layout && r->info.renderPass)
	{
		r->pi = &r->info;
		return r;
	}
	mlog_printf(r->ml, "[graph_gpipe_param_ok] (%s%s%s%s%s%s%s%s ) not set\n",
		r->info.stageCount?empty:" shader",
		r->info.pVertexInputState?empty:" vertex",
		r->info.pInputAssemblyState?empty:" assembly",
		r->info.pColorBlendState?empty:" color_blend",
		r->info.pDynamicState?empty:" dynamic",
		r->info.layout?empty:" layout",
		r->info.renderPass?empty:" render"
	);
	return NULL;
}
