#include "buffer_pri.h"
#include <alloca.h>

static void graph_memory_heap_free_func(register graph_memory_heap_s *restrict r)
{
	register void *v;
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_memory_heap_s* graph_memory_heap_alloc(register struct graph_dev_s *restrict dev)
{
	register graph_memory_heap_s *restrict r;
	r = (graph_memory_heap_s *) refer_alloz(sizeof(graph_memory_heap_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_memory_heap_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		vkGetPhysicalDeviceMemoryProperties(dev->phydev, &r->properties);
	}
	return r;
}

static VkDeviceMemory graph_memory_alloc(register graph_memory_heap_s *restrict r, register VkMemoryRequirements *restrict require, register graph_memory_property_t property)
{
	VkDeviceMemory memory;
	VkMemoryAllocateInfo info;
	VkResult ret;
	register uint32_t i;
	memory = NULL;
	info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	info.pNext = NULL;
	info.allocationSize = require->size;
	for (i = 0; i < r->properties.memoryTypeCount; ++i)
	{
		if ((require->memoryTypeBits & (1u << i)) &&
			(r->properties.memoryTypes[i].propertyFlags & property) == property)
			goto label_find_type;
	}
	mlog_printf(r->ml, "[graph_memory_alloc] do not find type @property = %02x\n", property);
	label_return_null:
	return NULL;
	label_find_type:
	info.memoryTypeIndex = i;
	ret = vkAllocateMemory(r->dev->dev, &info, &r->ga->alloc, &memory);
	if (!ret) return memory;
	mlog_printf(r->ml, "[graph_memory_alloc] vkAllocateMemory = %d\n", ret);
	goto label_return_null;
}

static void graph_buffer_free_func(register graph_buffer_s *restrict r)
{
	register void *v;
	if ((v = r->memory)) vkFreeMemory(r->dev->dev, (VkDeviceMemory) v, &r->ga->alloc);
	if ((v = r->buffer)) vkDestroyBuffer(r->dev->dev, (VkBuffer) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
	if ((v = r->heap)) refer_free(v);
}

graph_buffer_s* graph_buffer_alloc(register graph_memory_heap_s *restrict heap, uint64_t size, graph_buffer_flags_t flags, graph_buffer_usage_t usage, graph_memory_property_t property, uint32_t share_queue_number, const struct graph_device_queue_t *restrict share_queue_array)
{
	register graph_buffer_s *restrict r;
	VkBufferCreateInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.pNext = NULL;
	info.flags = (VkBufferCreateFlags) flags;
	info.size = size;
	info.usage = (VkBufferUsageFlags) usage;
	if (share_queue_number < 2)
	{
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.queueFamilyIndexCount = 0;
		info.pQueueFamilyIndices = NULL;
	}
	else
	{
		register uint32_t *queue;
		queue = (uint32_t *) alloca(sizeof(uint32_t) * share_queue_number);
		if (!queue) goto label_return_null;
		info.sharingMode = VK_SHARING_MODE_CONCURRENT;
		info.queueFamilyIndexCount = share_queue_number;
		info.pQueueFamilyIndices = queue;
		for (share_queue_number = 0; share_queue_number < info.queueFamilyIndexCount; ++info.queueFamilyIndexCount)
			queue[share_queue_number] = (uint32_t) share_queue_array[share_queue_number].index;
	}
	r = (graph_buffer_s *) refer_alloz(sizeof(graph_buffer_s));
	if (r)
	{
		VkDevice device;
		refer_set_free(r, (refer_free_f) graph_buffer_free_func);
		r->ml = (mlog_s *) refer_save(heap->ml);
		r->dev = (graph_dev_s *) refer_save(heap->dev);
		r->ga = (graph_allocator_s *) refer_save(heap->ga);
		r->heap = (graph_memory_heap_s *) refer_save(heap);
		ret = vkCreateBuffer(device = r->dev->dev, &info, &r->ga->alloc, &r->buffer);
		if (!ret)
		{
			vkGetBufferMemoryRequirements(device, r->buffer, &r->require);
			r->memory = graph_memory_alloc(heap, &r->require, property);
			if (r->memory)
			{
				ret = vkBindBufferMemory(device, r->buffer, r->memory, 0);
				if (!ret) return r;
				mlog_printf(r->ml, "[graph_buffer_alloc] vkBindBufferMemory = %d\n", ret);
			}
		}
		else mlog_printf(r->ml, "[graph_buffer_alloc] vkCreateBuffer = %d\n", ret);
		refer_free(r);
	}
	label_return_null:
	return NULL;
}

#define graph_buffer_range_fix(_bf, _off, _size)  if (_off > _bf->require.size) _off = _bf->require.size;\
						if (_size > (_bf->require.size - _off)) _size = (_bf->require.size - _off)

void* graph_buffer_map(register graph_buffer_s *restrict buffer, uint64_t offset, uint64_t size)
{
	void *r;
	VkResult ret;
	r = NULL;
	graph_buffer_range_fix(buffer, offset, size);
	if (size)
	{
		ret = vkMapMemory(buffer->dev->dev, buffer->memory, offset, size, 0, &r);
		if (!ret && r) return r;
		mlog_printf(buffer->ml, "[graph_buffer_map] vkMapMemory = %d\n", ret);
	}
	return NULL;
}

void graph_buffer_unmap(register graph_buffer_s *restrict buffer)
{
	vkUnmapMemory(buffer->dev->dev, buffer->memory);
}

graph_buffer_s* graph_buffer_flush(register graph_buffer_s *restrict buffer, uint64_t offset, uint64_t size)
{
	VkMappedMemoryRange range;
	VkResult ret;
	graph_buffer_range_fix(buffer, offset, size);
	if (size)
	{
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.pNext = NULL;
		range.memory = buffer->memory;
		range.offset = offset;
		range.size = size;
		ret = vkFlushMappedMemoryRanges(buffer->dev->dev, 1, &range);
		if (!ret) return buffer;
		mlog_printf(buffer->ml, "[graph_buffer_flush] vkFlushMappedMemoryRanges = %d\n", ret);
	}
	return NULL;
}

graph_buffer_s* graph_buffer_invalidate(register graph_buffer_s *restrict buffer, uint64_t offset, uint64_t size)
{
	VkMappedMemoryRange range;
	VkResult ret;
	graph_buffer_range_fix(buffer, offset, size);
	if (size)
	{
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.pNext = NULL;
		range.memory = buffer->memory;
		range.offset = offset;
		range.size = size;
		ret = vkInvalidateMappedMemoryRanges(buffer->dev->dev, 1, &range);
		if (!ret) return buffer;
		mlog_printf(buffer->ml, "[graph_buffer_invalidate] vkInvalidateMappedMemoryRanges = %d\n", ret);
	}
	return NULL;
}

#undef graph_buffer_range_fix
