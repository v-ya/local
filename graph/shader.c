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

static void graph_gpipe_param_free_func(register graph_gpipe_param_s *restrict r)
{
	register void *v;
	register graph_shader_stage_t *p;
	register uint32_t n;
	if ((v = r->viewports_scissors)) refer_free(v);
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
	if ((v = r->ga)) refer_free(v);
}

graph_gpipe_param_s* graph_gpipe_param_alloc(register struct graph_dev_s *restrict dev, uint32_t shader_number)
{
	register graph_gpipe_param_s *restrict r;
	if (shader_number)
	{
		r = (graph_gpipe_param_s *) refer_alloz(sizeof(graph_gpipe_param_s) +
			(sizeof(graph_shader_s *) + sizeof(VkPipelineShaderStageCreateInfo)) * shader_number);
		if (r)
		{
			refer_set_free(r, (refer_free_f) graph_gpipe_param_free_func);
			r->ml = (mlog_s *) refer_save(dev->ml);
			r->dev = (graph_dev_s *) refer_save(dev);
			r->ga = (graph_allocator_s *) refer_save(dev->ga);
			r->shader_number = shader_number;
			r->shader_stages = (graph_shader_stage_t *) (r + 1);
			r->shaders = (VkPipelineShaderStageCreateInfo *) (r->shader_stages + shader_number);
			r->info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			r->info.pStages = r->shaders;
			r->info.pRasterizationState = &r->rasterization;
			r->vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			r->input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			r->tessellation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			r->viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			r->rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			r->multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			r->depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			r->color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			r->dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
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
	r->rasterization.depthClampEnable = !!depth_clamp;
}

void graph_gpipe_param_set_rasterization_discard(register graph_gpipe_param_s *restrict r, graph_bool_t discard)
{
	r->rasterization.rasterizerDiscardEnable = !!discard;
}

void graph_gpipe_param_set_rasterization_polygon(register graph_gpipe_param_s *restrict r, graph_polygon_mode_t polygon)
{
	if ((uint32_t) polygon < graph_polygon_mode$number)
		r->rasterization.polygonMode = graph_polygon_mode2vk(polygon);
}

void graph_gpipe_param_set_rasterization_cull(register graph_gpipe_param_s *restrict r, graph_cull_mode_flags_t cull)
{
	r->rasterization.cullMode = (VkCullModeFlags) cull;
}

void graph_gpipe_param_set_rasterization_front_face(register graph_gpipe_param_s *restrict r, graph_front_face_t front_face)
{
	r->rasterization.frontFace = (VkFrontFace) front_face;
}

void graph_gpipe_param_set_rasterization_depth_bias(register graph_gpipe_param_s *restrict r, float constant_factor, float clamp, float slope_factor)
{
	r->rasterization.depthBiasEnable = 1;
	r->rasterization.depthBiasConstantFactor = constant_factor;
	r->rasterization.depthBiasClamp = clamp;
	r->rasterization.depthBiasSlopeFactor = slope_factor;
}

void graph_gpipe_param_set_rasterization_line_width(register graph_gpipe_param_s *restrict r, float line_width)
{
	r->rasterization.lineWidth = line_width;
}
