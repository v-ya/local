#include "layout_pri.h"
#include "type_pri.h"

static void graph_descriptor_set_layout_param_free_func(register graph_descriptor_set_layout_param_s *restrict r)
{
	register refer_t *restrict p;
	register refer_t v;
	register uint32_t n;
	p = (refer_t *) r->graph_sampler;
	n = r->sampler_number;
	while (n)
	{
		if ((v = p[--n]))
			refer_free(v);
	}
}

graph_descriptor_set_layout_param_s* graph_descriptor_set_layout_param_alloc(uint32_t binding_number, uint32_t sampler_number, graph_desc_set_layout_flags_t flags)
{
	register graph_descriptor_set_layout_param_s *restrict r;
	r = (graph_descriptor_set_layout_param_s *) refer_alloz(
		sizeof(graph_descriptor_set_layout_param_s) +
		sizeof(VkDescriptorSetLayoutBinding) * binding_number +
		(sizeof(struct graph_sampler_s *) + sizeof(VkSampler)) * sampler_number);
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_descriptor_set_layout_param_free_func);
		r->binding_size = binding_number;
		r->sampler_size = sampler_number;
		r->info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		r->info.flags = (VkDescriptorSetLayoutCreateFlags) flags;
		r->binding = (VkDescriptorSetLayoutBinding *) (r + 1);
		r->sampler = (VkSampler *) (r->binding + binding_number);
		r->graph_sampler = (struct graph_sampler_s **) (r->sampler + sampler_number);
		r->info.pBindings = r->binding;
	}
	return r;
}

graph_descriptor_set_layout_param_s* graph_descriptor_set_layout_param_set_binding(register graph_descriptor_set_layout_param_s *restrict param, uint32_t index, uint32_t binding, graph_desc_type_t type, uint32_t number, graph_shader_stage_flags_t flags)
{
	if (index < param->binding_size && (uint32_t) type < graph_desc_type$number)
	{
		register VkDescriptorSetLayoutBinding *restrict p;
		p = param->binding + index;
		p->binding = binding;
		p->descriptorType = graph_desc_type2vk(type);
		p->descriptorCount = number;
		p->stageFlags = (VkShaderStageFlags) flags;
		if (index >= param->binding_number)
			param->info.bindingCount = param->binding_number = index + 1;
		return param;
	}
	return NULL;
}

static void graph_descriptor_set_layout_free_func(register graph_descriptor_set_layout_s *restrict r)
{
	register void *v;
	if ((v = r->set_layout)) vkDestroyDescriptorSetLayout(r->dev->dev, (VkDescriptorSetLayout) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_descriptor_set_layout_s* graph_descriptor_set_layout_alloc(register graph_descriptor_set_layout_param_s *restrict param, register struct graph_dev_s *restrict dev)
{
	register graph_descriptor_set_layout_s *restrict r;
	VkResult ret;
	r = (graph_descriptor_set_layout_s *) refer_alloz(sizeof(graph_descriptor_set_layout_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_descriptor_set_layout_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		ret = vkCreateDescriptorSetLayout(dev->dev, &param->info, &r->ga->alloc, &r->set_layout);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_descriptor_set_layout_alloc] vkCreateDescriptorSetLayout = %d\n", ret);
		refer_free(r);
	}
	return NULL;
}

static void graph_pipe_layout_param_free_func(register graph_pipe_layout_param_s *restrict r)
{
	register refer_t *restrict p;
	register refer_t v;
	register uint32_t n;
	p = (refer_t *) r->rsl;
	n = r->set_layout_number;
	while (n)
	{
		if ((v = p[--n]))
			refer_free(v);
	}
}

graph_pipe_layout_param_s* graph_pipe_layout_param_alloc(uint32_t set_layout_number, uint32_t range_number)
{
	register graph_pipe_layout_param_s *restrict r;
	r = (graph_pipe_layout_param_s *) refer_alloz(
		sizeof(graph_pipe_layout_param_s) +
		(sizeof(struct graph_descriptor_set_layout_s *) + sizeof(VkDescriptorSetLayout)) * set_layout_number +
		sizeof(VkPushConstantRange) * range_number);
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_pipe_layout_param_free_func);
		r->set_layout_size = set_layout_number;
		r->push_constant_range_size = range_number;
		r->rsl = (graph_descriptor_set_layout_s **) (r + 1);
		r->set_layouts = (VkDescriptorSetLayout *) (r->rsl + set_layout_number);
		r->push_constant_ranges = (VkPushConstantRange *) (r->set_layouts + set_layout_number);
	}
	return r;
}

graph_pipe_layout_param_s* graph_pipe_layout_param_append_set_layout(register graph_pipe_layout_param_s *restrict param, register const graph_descriptor_set_layout_s *restrict set_layout)
{
	register uint32_t i;
	if ((i = param->set_layout_number) < param->set_layout_size)
	{
		param->rsl[i] = (graph_descriptor_set_layout_s *) refer_save(set_layout);
		param->set_layouts[i] = set_layout->set_layout;
		param->set_layout_number = i + 1;
		return param;
	}
	return NULL;
}

graph_pipe_layout_param_s* graph_pipe_layout_param_append_range(register graph_pipe_layout_param_s *restrict param, graph_shader_stage_flags_t flags, uint32_t offset, uint32_t size)
{
	register VkPushConstantRange *restrict p;
	register uint32_t i;
	if ((i = param->push_constant_range_number) < param->push_constant_range_size)
	{
		p = param->push_constant_ranges + i;
		p->stageFlags = (VkShaderStageFlags) flags;
		p->offset = offset;
		p->size = size;
		param->push_constant_range_number = i + 1;
		return param;
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
			info.setLayoutCount = param->set_layout_number;
			info.pSetLayouts = param->set_layouts;
			info.pushConstantRangeCount = param->push_constant_range_number;
			info.pPushConstantRanges = param->push_constant_ranges;
		}
		ret = vkCreatePipelineLayout(dev->dev, &info, &r->ga->alloc, &r->layout);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_pipe_layout_alloc] vkCreatePipelineLayout = %d\n", ret);
		refer_free(r);
	}
	return NULL;
}

