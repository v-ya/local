#include "worker.inner.h"

static void iphyee_worker_buffer_free_func(iphyee_worker_buffer_s *restrict r)
{
	if (r->buffer) vkDestroyBuffer(r->device, r->buffer, NULL);
	if (r->memory) vkFreeMemory(r->device, r->memory, NULL);
	if (r->memory_heap) refer_free(r->memory_heap);
}

iphyee_worker_buffer_s* iphyee_worker_buffer_alloc(iphyee_worker_memory_heap_s *restrict memory_heap, uint64_t size, VkBufferCreateFlags flags, VkBufferUsageFlags usage)
{
	iphyee_worker_buffer_s *restrict r;
	VkBufferCreateInfo info;
	VkMemoryAllocateInfo info_memory;
	VkMemoryAllocateFlagsInfo info_memory_flags;
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.pNext = NULL;
	info.flags = flags;
	info.size = size;
	info.usage = usage;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount = 0;
	info.pQueueFamilyIndices = NULL;
	if ((r = (iphyee_worker_buffer_s *) refer_alloz(sizeof(iphyee_worker_buffer_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_buffer_free_func);
		r->memory_heap = (iphyee_worker_memory_heap_s *) refer_save(memory_heap);
		r->device = memory_heap->device;
		if (!vkCreateBuffer(r->device, &info, NULL, &r->buffer))
		{
			vkGetBufferMemoryRequirements(r->device, r->buffer, &r->require);
			info_memory.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			info_memory.pNext = NULL;
			info_memory.allocationSize = r->require.size;
			info_memory.memoryTypeIndex = memory_heap->memory_type_index;
			if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
			{
				info_memory_flags.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
				info_memory_flags.pNext = NULL;
				info_memory_flags.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
				info_memory_flags.deviceMask = 0;
				info_memory.pNext = &info_memory_flags;
			}
			if (!vkAllocateMemory(r->device, &info_memory, NULL, &r->memory) &&
				!vkBindBufferMemory(r->device, r->buffer, r->memory, 0))
				return r;
		}
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_buffer_s* iphyee_worker_buffer_create_storage(iphyee_worker_memory_heap_s *restrict memory_heap, uint64_t size, iphyee_worker_buffer_usage_t usage)
{
	VkBufferUsageFlags vk_usage;
	vk_usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | (usage & iphyee_worker_buffer_usage__fix);
	if (memory_heap->heap->flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
		vk_usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	return iphyee_worker_buffer_alloc(memory_heap, size, 0, vk_usage);
}

uint64_t iphyee_worker_buffer_device_address(iphyee_worker_buffer_s *restrict buffer)
{
	VkBufferDeviceAddressInfo info;
	info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	info.pNext = NULL;
	info.buffer = buffer->buffer;
	return vkGetBufferDeviceAddress(buffer->device, &info);
}

void* iphyee_worker_buffer_map(iphyee_worker_buffer_s *restrict buffer, uint64_t offset, uint64_t size)
{
	void *ptr;
	if (!vkMapMemory(buffer->device, buffer->memory, offset, size, 0, &ptr))
		return ptr;
	return NULL;
}

void iphyee_worker_buffer_unmap(iphyee_worker_buffer_s *restrict buffer)
{
	vkUnmapMemory(buffer->device, buffer->memory);
}

iphyee_worker_buffer_s* iphyee_worker_buffer_flush(iphyee_worker_buffer_s *restrict buffer, uint64_t offset, uint64_t size)
{
	VkMappedMemoryRange memory_range;
	memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	memory_range.pNext = NULL;
	memory_range.memory = buffer->memory;
	memory_range.offset = offset;
	memory_range.size = size;
	if (!vkFlushMappedMemoryRanges(buffer->device, 1, &memory_range))
		return buffer;
	return NULL;
}

iphyee_worker_buffer_s* iphyee_worker_buffer_invalidate(iphyee_worker_buffer_s *restrict buffer, uint64_t offset, uint64_t size)
{
	VkMappedMemoryRange memory_range;
	memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	memory_range.pNext = NULL;
	memory_range.memory = buffer->memory;
	memory_range.offset = offset;
	memory_range.size = size;
	if (!vkInvalidateMappedMemoryRanges(buffer->device, 1, &memory_range))
		return buffer;
	return NULL;
}
