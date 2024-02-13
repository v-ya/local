#include "worker.inner.h"

static void iphyee_worker_free_func(iphyee_worker_s *restrict r)
{
	if (r->device) iphyee_worker_device_wait_idle(r->device);
	if (r->syncpool) refer_free(r->syncpool);
	if (r->mheap_device) refer_free(r->mheap_device);
	if (r->mheap_host) refer_free(r->mheap_host);
	if (r->device) refer_free(r->device);
}

iphyee_worker_s* iphyee_worker_alloc(iphyee_worker_instance_s *restrict instance, uintptr_t index, uint32_t compute_submit_count, uint32_t transfer_submit_count)
{
	iphyee_worker_s *restrict r;
	if (instance && (r = (iphyee_worker_s *) refer_alloz(sizeof(iphyee_worker_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_free_func);
		if ((r->syncpool = iphyee_worker_syncpool_alloc()) &&
			(r->device = iphyee_worker_instance_create_device(instance, index,
				compute_submit_count, transfer_submit_count)) &&
			(r->mheap_host = iphyee_worker_memory_heap_alloc(r->device,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) &&
			(r->mheap_device = iphyee_worker_memory_heap_alloc(r->device,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)))
			return r;
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_s* iphyee_worker_insert_semaphore(iphyee_worker_s *restrict r, const char *restrict semaphore_name, uintptr_t semaphore_count)
{
	if (iphyee_worker_syncpool_insert_semaphore(r->syncpool, r->device, semaphore_name, semaphore_count))
		return r;
	return NULL;
}

iphyee_worker_s* iphyee_worker_insert_fence(iphyee_worker_s *restrict r, const char *restrict fence_name, uintptr_t fence_count)
{
	if (iphyee_worker_syncpool_insert_fence(r->syncpool, r->device, fence_name, fence_count))
		return r;
	return NULL;
}

iphyee_worker_shader_s* iphyee_worker_create_shader(iphyee_worker_s *restrict r, const void *restrict code, uintptr_t size, const char *restrict entry_name, uint32_t push_constants_size)
{
	return iphyee_worker_shader_alloc(r->device, code, size, entry_name, push_constants_size);
}

iphyee_worker_buffer_s* iphyee_worker_create_buffer_host(iphyee_worker_s *restrict r, uint64_t size, iphyee_worker_buffer_usage_t usage)
{
	return iphyee_worker_buffer_create_storage(r->mheap_host, size, usage);
}

iphyee_worker_buffer_s* iphyee_worker_create_buffer_device(iphyee_worker_s *restrict r, uint64_t size, iphyee_worker_buffer_usage_t usage)
{
	return iphyee_worker_buffer_create_storage(r->mheap_device, size, usage);
}

iphyee_worker_s* iphyee_worker_wait_fence(iphyee_worker_s *restrict r, const char *restrict fence_name, uintptr_t fence_index, uint64_t timeout_nsec)
{
	iphyee_worker_fence_s *restrict fence;
	if ((fence = iphyee_worker_syncpool_save_fence(r->syncpool, fence_name, fence_index)))
	{
		if (!iphyee_worker_fence_wait(fence, timeout_nsec))
			r = NULL;
		refer_free(fence);
		return r;
	}
	return NULL;
}

iphyee_worker_s* iphyee_worker_wait_idle(iphyee_worker_s *restrict r)
{
	if (iphyee_worker_device_wait_idle(r->device))
		return r;
	return NULL;
}
