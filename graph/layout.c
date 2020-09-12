#include "layout_pri.h"
#include "buffer_pri.h"
#include "type_pri.h"
#include <memory.h>
#include <alloca.h>

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

static void graph_descriptor_pool_free_func(register graph_descriptor_pool_s *restrict r)
{
	register void *v;
	if ((v = r->pool)) vkDestroyDescriptorPool(r->dev->dev, (VkDescriptorPool) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_descriptor_pool_s* graph_descriptor_pool_alloc(register struct graph_dev_s *restrict dev, graph_desc_pool_flags_t flags, uint32_t max_sets, uint32_t pool_size_number, graph_desc_type_t pool_type[], uint32_t pool_desc_number[])
{
	register graph_descriptor_pool_s *restrict r;
	register VkDescriptorPoolSize *restrict p;
	VkDescriptorPoolCreateInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.pNext = NULL;
	info.flags = (VkDescriptorPoolCreateFlags) flags;
	info.maxSets = max_sets;
	info.poolSizeCount = pool_size_number;
	info.pPoolSizes = NULL;
	if (pool_size_number)
	{
		p = (VkDescriptorPoolSize *) alloca(sizeof(VkDescriptorPoolSize) * pool_size_number);
		if (!p) goto label_fail;
		for (max_sets = 0; max_sets < pool_size_number; ++max_sets)
		{
			if (pool_type[max_sets] >= graph_desc_type$number)
				goto label_type;
			p->type = graph_desc_type2vk(pool_type[max_sets]);
			p->descriptorCount = pool_desc_number[max_sets];
		}
		info.pPoolSizes = p;
	}
	r = (graph_descriptor_pool_s *) refer_alloz(sizeof(graph_descriptor_pool_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_descriptor_pool_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		ret = vkCreateDescriptorPool(dev->dev, &info, &r->ga->alloc, &r->pool);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_descriptor_pool_alloc] vkCreateDescriptorPool = %d\n", ret);
		refer_free(r);
	}
	label_fail:
	return NULL;
	label_type:
	mlog_printf(dev->ml, "[graph_descriptor_pool_alloc] not find pool_type[%u] = %d\n", max_sets, pool_type[max_sets]);
	goto label_fail;
}

static void graph_descriptor_sets_free_func(register graph_descriptor_sets_s *restrict r)
{
	register refer_t v;
	register uint32_t i;
	if (r->pool) refer_free(r->pool);
	for (i = 0; i < r->number; ++i)
	{
		if ((v = r->layout[i]))
			refer_free(v);
	}
}

graph_descriptor_sets_s* graph_descriptor_sets_alloc(register graph_descriptor_pool_s *restrict pool, uint32_t number, graph_descriptor_set_layout_s *set_layout[])
{
	register graph_descriptor_sets_s *restrict r;
	register VkDescriptorSetLayout *restrict p;
	VkDescriptorSetAllocateInfo info;
	uint32_t i;
	VkResult ret;
	if (number)
	{
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		info.pNext = NULL;
		info.descriptorPool = pool->pool;
		info.descriptorSetCount = number;
		p = (VkDescriptorSetLayout *) alloca(sizeof(VkDescriptorSetLayout) * number);
		if (p)
		{
			info.pSetLayouts = p;
			r = (graph_descriptor_sets_s *) refer_alloc(sizeof(graph_descriptor_sets_s) +
				(sizeof(graph_descriptor_set_layout_s *) + sizeof(VkDescriptorSet)) * number);
			if (r)
			{
				refer_set_free(r, (refer_free_f) graph_descriptor_sets_free_func);
				r->pool = (graph_descriptor_pool_s *) refer_save(pool);
				r->number = number;
				r->res = 0;
				r->layout = (graph_descriptor_set_layout_s **) (r + 1);
				r->set = (VkDescriptorSet *) (r->layout + number);
				for (i = 0; i < number; ++i)
				{
					p[i] = set_layout[i]->set_layout;
					r->layout[i] = (graph_descriptor_set_layout_s *) refer_save(set_layout[i]);
					r->set[i] = NULL;
				}
				ret = vkAllocateDescriptorSets(pool->dev->dev, &info, r->set);
				if (!ret) return r;
				mlog_printf(pool->ml, "[graph_descriptor_set_alloc] vkAllocateDescriptorSets = %d\n", ret);
				refer_free(r);
			}
		}
	}
	return NULL;
}

static void graph_descriptor_sets_info_free_func(register graph_descriptor_sets_info_s *restrict r)
{
	register void *v;
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->sets)) refer_free(v);
}

graph_descriptor_sets_info_s* graph_descriptor_sets_info_alloc(register graph_descriptor_sets_s *restrict sets, uint32_t n_write, uint32_t n_copy, uint32_t n_image_info, uint32_t n_buffer_info, uint32_t n_buffer_view)
{
	register graph_descriptor_sets_info_s *restrict r;
	size_t size;
	size = sizeof(graph_descriptor_sets_info_s);
	size += sizeof(VkWriteDescriptorSet) * n_write;
	size += sizeof(VkCopyDescriptorSet) * n_copy;
	size += sizeof(VkDescriptorImageInfo) * n_image_info;
	size += sizeof(VkDescriptorBufferInfo) * n_buffer_info;
	size += sizeof(VkBufferView) * n_buffer_view;
	r = (graph_descriptor_sets_info_s *) refer_alloc(size);
	if (r)
	{
		r->ml = (mlog_s *) refer_save(sets->pool->ml);
		r->dev = (graph_dev_s *) refer_save(sets->pool->dev);
		r->sets = (graph_descriptor_sets_s *) refer_save(sets);
		r->write = (VkWriteDescriptorSet *) (r + 1);
		r->copy = (VkCopyDescriptorSet *) (r->write + n_write);
		r->image = (VkDescriptorImageInfo *) (r->copy + n_copy);
		r->buffer = (VkDescriptorBufferInfo *) (r->image + n_image_info);
		r->view = (VkBufferView *) (r->buffer + n_buffer_info);
		r->write_size = n_write;
		r->copy_size = n_copy;
		r->write_number = 0;
		r->copy_number = 0;
		r->image_size = n_image_info;
		r->buffer_size = n_buffer_info;
		r->view_size = n_buffer_view;
		r->res = 0;
		refer_set_free(r, (refer_free_f) graph_descriptor_sets_info_free_func);
	}
	return r;
}

uint32_t graph_descriptor_sets_info_append_write(register graph_descriptor_sets_info_s *restrict info, uint32_t set_index, uint32_t binding, graph_desc_type_t type, uint32_t offset, uint32_t count)
{
	register VkWriteDescriptorSet *restrict p;
	register void *restrict v;
	uint32_t r;
	if ((r = info->write_number) < info->write_size && set_index < info->sets->number && count)
	{
		p = info->write + r;
		p->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		p->pNext = NULL;
		p->dstSet = info->sets->set[set_index];
		p->dstBinding = binding;
		p->dstArrayElement = offset;
		p->descriptorCount = count;
		p->descriptorType = graph_desc_type2vk(type);
		p->pImageInfo = NULL;
		p->pBufferInfo = NULL;
		p->pTexelBufferView = NULL;
		switch (type)
		{
			case graph_desc_type_uniform_buffer:
				if (info->buffer_size < count)
					goto label_fail;
				p->pBufferInfo = v = info->buffer + (info->buffer_size -= count);
				memset(v, 0, sizeof(VkDescriptorBufferInfo) * count);
				break;
			default:
				goto label_fail;
		}
		info->write_number = r + 1;
		return r;
	}
	label_fail:
	return ~0;
}

void graph_descriptor_sets_info_set_write_buffer_info(register graph_descriptor_sets_info_s *restrict info, uint32_t write_index, uint32_t buffer_info_index, const struct graph_buffer_s *restrict buffer, uint64_t offset, uint64_t size)
{
	register VkWriteDescriptorSet *restrict w;
	register VkDescriptorBufferInfo *restrict p;
	if (write_index < info->write_number && buffer_info_index < (w = info->write + write_index)->descriptorCount)
	{
		p = (VkDescriptorBufferInfo *) w->pBufferInfo + buffer_info_index;
		p->buffer = buffer->buffer;
		if (offset > buffer->require.size) offset = buffer->require.size;
		if (size > buffer->require.size - offset) size = buffer->require.size - offset;
		p->offset = offset;
		p->range = size;
	}
}

void graph_descriptor_sets_info_update(register graph_descriptor_sets_info_s *restrict info)
{
	vkUpdateDescriptorSets(info->dev->dev,
		info->write_number, info->write_number?info->write:NULL,
		info->copy_number, info->copy_number?info->copy:NULL);
}
