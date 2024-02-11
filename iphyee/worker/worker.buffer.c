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
			if (!vkAllocateMemory(r->device, &info_memory, NULL, &r->memory) &&
				!vkBindBufferMemory(r->device, r->buffer, r->memory, 0))
				return r;
		}
		refer_free(r);
	}
	return NULL;
}
