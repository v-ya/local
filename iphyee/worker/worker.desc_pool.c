#include "worker.inner.h"

static void iphyee_worker_desc_pool_free_func(iphyee_worker_desc_pool_s *restrict r)
{
	uintptr_t i, n;
	if (r->desc_pool) vkDestroyDescriptorPool(r->device, r->desc_pool, NULL);
	n = r->sets_cur_count;
	for (i = 0; i < n; ++i)
	{
		if (r->setlayout_array[i])
			refer_free(r->setlayout_array[i]);
	}
	if (r->depend) refer_free(r->depend);
}

iphyee_worker_desc_pool_s* iphyee_worker_desc_pool_alloc(iphyee_worker_device_s *restrict device, uintptr_t sets_max_count, uintptr_t size_count, const VkDescriptorPoolSize *restrict size_array)
{
	iphyee_worker_desc_pool_s *restrict r;
	VkDescriptorPoolCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.maxSets = (uint32_t) sets_max_count;
	info.poolSizeCount = size_count;
	info.pPoolSizes = size_array;
	if ((r = (iphyee_worker_desc_pool_s *) refer_alloz(sizeof(iphyee_worker_desc_pool_s) +
		sizeof(iphyee_worker_setlayout_s *) * sets_max_count)))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_desc_pool_free_func);
		r->depend = (iphyee_worker_device_s *) refer_save(device);
		r->device = device->device;
		r->sets_max_count = sets_max_count;
		if (!vkCreateDescriptorPool(r->device, &info, NULL, &r->desc_pool))
			return r;
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_desc_pool_s* iphyee_worker_desc_pool_reset(iphyee_worker_desc_pool_s *restrict r)
{
	uintptr_t i, n;
	if (!vkResetDescriptorPool(r->device, r->desc_pool, 0))
	{
		n = r->sets_cur_count;
		for (i = 0; i < n; ++i)
		{
			if (r->setlayout_array[i])
				refer_free(r->setlayout_array[i]);
		}
		r->sets_cur_count = 0;
		return r;
	}
	return NULL;
}

iphyee_worker_desc_pool_s* iphyee_worker_desc_pool_get(iphyee_worker_desc_pool_s *restrict r, uintptr_t number, iphyee_worker_setlayout_s *const *restrict setlayout_array, VkDescriptorSet *restrict desc_sets_array, const iphyee_worker_desc_set_wbuffs_t *restrict wbuffs_array)
{
	VkDescriptorSetAllocateInfo info;
	VkDescriptorSetLayout layouts[number];
	iphyee_worker_setlayout_s *restrict sl;
	const iphyee_worker_desc_set_buffer_t *restrict psb;
	uintptr_t wcount;
	uintptr_t i, j, k, m;
	if (number && r->sets_cur_count + number <= r->sets_max_count)
	{
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		info.pNext = NULL;
		info.descriptorPool = r->desc_pool;
		info.descriptorSetCount = number;
		info.pSetLayouts = layouts;
		wcount = 0;
		for (i = 0; i < number; ++i)
		{
			if (refer_save(r->setlayout_array[r->sets_cur_count++] = sl = setlayout_array[i]))
				layouts[i] = sl->setlayout;
			else layouts[i] = NULL;
		}
		if (wbuffs_array)
		{
			for (i = 0; i < number; ++i)
				wcount += wbuffs_array[i].buffer_count;
		}
		if (!vkAllocateDescriptorSets(r->device, &info, desc_sets_array))
		{
			if (wcount)
			{
				VkWriteDescriptorSet wset[wcount];
				VkDescriptorBufferInfo wbi[wcount];
				for (i = k = 0; i < number; ++i)
				{
					psb = wbuffs_array[i].buffer_array;
					m = wbuffs_array[i].buffer_count;
					for (j = 0; i < m; ++i)
					{
						if (psb[j].buffer)
						{
							wset[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							wset[k].pNext = NULL;
							wset[k].dstSet = desc_sets_array[i];
							wset[k].dstBinding = psb[j].binding;
							wset[k].dstArrayElement = 0;
							wset[k].descriptorCount = 1;
							wset[k].descriptorType = psb[j].desc_type;
							wset[k].pImageInfo = NULL;
							wset[k].pBufferInfo = wbi + k;
							wset[k].pTexelBufferView = NULL;
							wbi[k].buffer = psb[j].buffer->buffer;
							wbi[k].offset = psb[j].offset;
							wbi[k].range = psb[j].length;
							k += 1;
						}
					}
				}
				if (k) vkUpdateDescriptorSets(r->device, k, wset, 0, NULL);
			}
			return r;
		}
	}
	return NULL;
}

void iphyee_worker_desc_set_write_buffer(iphyee_worker_desc_pool_s *restrict r, VkDescriptorSet desc_set, const iphyee_worker_desc_set_buffer_t *restrict buffer_array, uintptr_t buffer_count)
{
	VkWriteDescriptorSet wset[buffer_count];
	VkDescriptorBufferInfo wbi[buffer_count];
	uintptr_t i, k;
	for (i = k = 0; i < buffer_count; ++i)
	{
		if (buffer_array[i].buffer)
		{
			wset[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			wset[k].pNext = NULL;
			wset[k].dstSet = desc_set;
			wset[k].dstBinding = buffer_array[i].binding;
			wset[k].dstArrayElement = 0;
			wset[k].descriptorCount = 1;
			wset[k].descriptorType = buffer_array[i].desc_type;
			wset[k].pImageInfo = NULL;
			wset[k].pBufferInfo = wbi + k;
			wset[k].pTexelBufferView = NULL;
			wbi[k].buffer = buffer_array[i].buffer->buffer;
			wbi[k].offset = buffer_array[i].offset;
			wbi[k].range = buffer_array[i].length;
			k += 1;
		}
	}
	if (k) vkUpdateDescriptorSets(r->device, k, wset, 0, NULL);
}
