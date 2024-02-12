#include "worker.inner.h"

static void iphyee_worker_memory_heap_free_func(iphyee_worker_memory_heap_s *restrict r)
{
	if (r->depend) refer_free(r->depend);
}

iphyee_worker_memory_heap_s* iphyee_worker_memory_heap_alloc(iphyee_worker_device_s *restrict device, VkMemoryPropertyFlags flags)
{
	iphyee_worker_memory_heap_s *restrict r;
	const VkMemoryType *restrict p;
	uint32_t i, n;
	p = device->memory_properties.memoryTypes;
	n = device->memory_properties.memoryTypeCount;
	for (i = 0; i < n; ++i)
	{
		if ((p[i].propertyFlags & flags) == flags)
		{
			if (((r = (iphyee_worker_memory_heap_s *) refer_alloz(sizeof(iphyee_worker_memory_heap_s)))))
			{
				refer_set_free(r, (refer_free_f) iphyee_worker_memory_heap_free_func);
				r->device = device->device;
				r->memory_type_index = i;
				r->memory_heap_index = p[i].heapIndex;
				r->depend = (iphyee_worker_device_s *) refer_save(device);
				r->heap = device->memory_properties.memoryHeaps + r->memory_heap_index;
				return r;
			}
			break;
		}
	}
	return NULL;
}
