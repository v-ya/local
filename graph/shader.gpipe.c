#include "shader_pri.h"
#include "layout_pri.h"
#include "type_pri.h"
#include <stdlib.h>

graph_pipe_s* graph_pipe_alloc_graphics(const graph_gpipe_param_s *restrict param, const graph_pipe_cache_s *restrict cache)
{
	graph_pipe_s *restrict r;
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

static void graph_gpipe_param_free_func(graph_gpipe_param_s *restrict r)
{
	void *v;
	graph_pipe_shader_stage_t *p;
	uint32_t n;
	if ((v = (void *) r->dynamic.pDynamicStates)) free(v);
	if ((v = r->vertex)) refer_free(v);
	if ((v = r->viewports_scissors)) refer_free(v);
	if ((v = r->color_blend)) refer_free(v);
	if ((v = r->pipe_layout)) refer_free(v);
	if ((v = r->render)) refer_free(v);
	p = r->shader_stages;
	n = r->shader_number;
	do {
		graph_pipe_shader_stage_finally(p);
		++p;
	} while (--n);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
}

graph_gpipe_param_s* graph_gpipe_param_alloc(struct graph_dev_s *restrict dev, uint32_t shader_number)
{
	graph_gpipe_param_s *restrict r;
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

graph_gpipe_param_s* graph_gpipe_param_append_shader(graph_gpipe_param_s *restrict r, const graph_shader_s *restrict shader, graph_shader_type_t type, const char *restrict entry, const graph_shader_spec_s *restrict spec)
{
	VkPipelineShaderStageCreateInfo *restrict p;
	graph_pipe_shader_stage_t *restrict s;
	uint32_t i;
	r->pi = NULL;
	if (!shader || !entry) goto label_arg;
	if ((i = r->info.stageCount) >= r->shader_number) goto label_shader_max;
	p = r->shaders + i;
	s = r->shader_stages + i;
	if (graph_pipe_shader_stage_initial(s, shader, spec, entry))
	{
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

void graph_gpipe_param_set_vertex(graph_gpipe_param_s *restrict r, const graph_vertex_input_description_s *restrict vertex)
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

graph_gpipe_param_s* graph_gpipe_param_set_assembly(graph_gpipe_param_s *restrict r, graph_primitive_topology_t pt, graph_bool_t primitive_restart)
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

graph_gpipe_param_s* graph_gpipe_param_set_viewport(graph_gpipe_param_s *restrict r, const graph_viewports_scissors_s *restrict gvs)
{
	r->pi = NULL;
	r->info.pViewportState = NULL;
	if (r->viewports_scissors)
	{
		refer_free(r->viewports_scissors);
		r->viewports_scissors = NULL;
	}
	if (gvs)
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

graph_gpipe_param_s* graph_gpipe_param_set_color_blend(graph_gpipe_param_s *restrict r, const graph_pipe_color_blend_s *restrict blend)
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

void graph_gpipe_param_set_rasterization_depth_clamp(graph_gpipe_param_s *restrict r, graph_bool_t depth_clamp)
{
	r->pi = NULL;
	r->rasterization.depthClampEnable = !!depth_clamp;
}

void graph_gpipe_param_set_rasterization_discard(graph_gpipe_param_s *restrict r, graph_bool_t discard)
{
	r->pi = NULL;
	r->rasterization.rasterizerDiscardEnable = !!discard;
}

void graph_gpipe_param_set_rasterization_polygon(graph_gpipe_param_s *restrict r, graph_polygon_mode_t polygon)
{
	r->pi = NULL;
	if ((uint32_t) polygon < graph_polygon_mode$number)
		r->rasterization.polygonMode = graph_polygon_mode2vk(polygon);
}

void graph_gpipe_param_set_rasterization_cull(graph_gpipe_param_s *restrict r, graph_cull_mode_flags_t cull)
{
	r->pi = NULL;
	r->rasterization.cullMode = (VkCullModeFlags) cull;
}

void graph_gpipe_param_set_rasterization_front_face(graph_gpipe_param_s *restrict r, graph_front_face_t front_face)
{
	r->pi = NULL;
	r->rasterization.frontFace = (VkFrontFace) front_face;
}

void graph_gpipe_param_set_rasterization_depth_bias(graph_gpipe_param_s *restrict r, float constant_factor, float clamp, float slope_factor)
{
	r->pi = NULL;
	r->rasterization.depthBiasEnable = 1;
	r->rasterization.depthBiasConstantFactor = constant_factor;
	r->rasterization.depthBiasClamp = clamp;
	r->rasterization.depthBiasSlopeFactor = slope_factor;
}

void graph_gpipe_param_set_rasterization_line_width(graph_gpipe_param_s *restrict r, float line_width)
{
	r->pi = NULL;
	r->rasterization.lineWidth = line_width;
}

void graph_gpipe_param_set_multisample_sample_count(graph_gpipe_param_s *restrict r, graph_sample_count_t sample_count)
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

void graph_gpipe_param_set_multisample_sample_shading(graph_gpipe_param_s *restrict r, graph_bool_t sample_shading)
{
	r->pi = NULL;
	r->multisample.sampleShadingEnable = !!sample_shading;
}

void graph_gpipe_param_set_multisample_min_sample_shading(graph_gpipe_param_s *restrict r, float min_sample_shading)
{
	r->pi = NULL;
	r->multisample.minSampleShading = min_sample_shading;
}

void graph_gpipe_param_set_multisample_alpha2coverage(graph_gpipe_param_s *restrict r, graph_bool_t alpha2coverage)
{
	r->pi = NULL;
	r->multisample.alphaToCoverageEnable = !!alpha2coverage;
}

void graph_gpipe_param_set_multisample_alpha2one(graph_gpipe_param_s *restrict r, graph_bool_t alpha2one)
{
	r->pi = NULL;
	r->multisample.alphaToOneEnable = !!alpha2one;
}

graph_gpipe_param_s* graph_gpipe_param_set_dynamic(graph_gpipe_param_s *restrict r, uint32_t n, const graph_dynamic_t dynamic[])
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

void graph_gpipe_param_set_layout(graph_gpipe_param_s *restrict r, const struct graph_pipe_layout_s *restrict layout)
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

void graph_gpipe_param_set_render(graph_gpipe_param_s *restrict r, const graph_render_pass_s *restrict render, uint32_t subpass_index)
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

graph_gpipe_param_s* graph_gpipe_param_ok(graph_gpipe_param_s *restrict r)
{
	static char empty[] = {0};
	r->pi = NULL;
	if (r->info.stageCount && r->info.pVertexInputState && r->info.pInputAssemblyState && r->info.pViewportState &&
		r->info.pColorBlendState && r->info.pDynamicState && r->info.layout && r->info.renderPass)
	{
		r->pi = &r->info;
		return r;
	}
	mlog_printf(r->ml, "[graph_gpipe_param_ok] (%s%s%s%s%s%s%s%s ) not set\n",
		r->info.stageCount?empty:" shader",
		r->info.pVertexInputState?empty:" vertex",
		r->info.pInputAssemblyState?empty:" assembly",
		r->info.pViewportState?empty:" viewport",
		r->info.pColorBlendState?empty:" color_blend",
		r->info.pDynamicState?empty:" dynamic",
		r->info.layout?empty:" layout",
		r->info.renderPass?empty:" render"
	);
	return NULL;
}
