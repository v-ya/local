#include "shader_pri.h"
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

void graph_pipe_layout_free_func(register graph_pipe_layout_s *restrict r)
{
	register void *v;
	if ((v = r->layout)) vkDestroyPipelineLayout(r->dev->dev, (VkPipelineLayout) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_pipe_layout_s* graph_pipe_layout_alloc(register struct graph_dev_s *restrict dev, register const graph_pipe_layout_param_s *restrict param)
{
	register graph_pipe_layout_s *restrict r;
	VkPipelineLayoutCreateInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.setLayoutCount = 0;
	info.pSetLayouts = NULL;
	info.pushConstantRangeCount = 0;
	info.pPushConstantRanges = NULL;
	r = (graph_pipe_layout_s *) refer_alloz(sizeof(graph_pipe_layout_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_pipe_layout_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		if (param)
		{
			;
		}
		ret = vkCreatePipelineLayout(dev->dev, &info, &r->ga->alloc, &r->layout);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_pipe_layout_alloc] vkCreatePipelineLayout = %d\n", ret);
		refer_free(r);
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
	if (index < r->sp_size && n < r->ar_size)
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

// graph_pipe_s* graph_pipe_alloc_graphics(register graph_gpipe_param_s *restrict param)
// {
// 	// VkGraphicsPipelineCreateInfo;
// 	// vkCreateGraphicsPipelines(, , , , , );
// 	return NULL;
// }

static void graph_gpipe_param_free_func(register graph_gpipe_param_s *restrict r)
{
	register void *v;
	register graph_shader_stage_t *p;
	register uint32_t n;
	if ((v = r->viewports_scissors)) refer_free(v);
	if ((v = r->pipe_layout)) refer_free(v);
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
			(sizeof(graph_shader_stage_t) + sizeof(VkPipelineShaderStageCreateInfo)) * shader_number);
		if (r)
		{
			refer_set_free(r, (refer_free_f) graph_gpipe_param_free_func);
			r->ml = (mlog_s *) refer_save(dev->ml);
			r->dev = (graph_dev_s *) refer_save(dev);
			r->shader_number = shader_number;
			r->shader_stages = (graph_shader_stage_t *) (r + 1);
			r->shaders = (VkPipelineShaderStageCreateInfo *) (r->shader_stages + shader_number);
			r->info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			r->info.pStages = r->shaders;
			r->info.pRasterizationState = &r->rasterization;
			r->info.pMultisampleState = &r->multisample;
			r->info.pColorBlendState = &r->color_blend;
			r->vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			r->input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			r->tessellation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			r->viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			r->rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			r->multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			r->depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			r->color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
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
	register graph_shader_stage_t *restrict s;
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

graph_gpipe_param_s* graph_gpipe_param_set_assembly(register graph_gpipe_param_s *restrict r, graph_primitive_topology_t pt, graph_bool_t primitive_restart)
{
	r->pi = NULL;
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
	if (r->viewports_scissors)
	{
		refer_free(r->viewports_scissors);
		r->viewports_scissors = NULL;
	}
	r->info.pViewportState = NULL;
	if (gvs && gvs->viewport_number && gvs->scissor_number)
	{
		r->viewport.flags = 0;
		r->viewports_scissors = (graph_viewports_scissors_s *) refer_save(gvs);
		r->viewport.viewportCount = gvs->viewport_number;
		r->viewport.pViewports = gvs->viewports;
		r->viewport.scissorCount = gvs->scissor_number;
		r->viewport.pScissors = gvs->scissors;
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

void graph_gpipe_param_set_layout(register graph_gpipe_param_s *restrict r, register const graph_pipe_layout_s *restrict layout)
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
