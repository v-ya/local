#include "worker.inner.h"

static void iphyee_worker_free_func(iphyee_worker_s *restrict r)
{
	if (r->device) iphyee_worker_device_wait_idle(r->device);
	if (r->mheap_device) refer_free(r->mheap_device);
	if (r->mheap_host) refer_free(r->mheap_host);
	if (r->queue_transfer) refer_free(r->queue_transfer);
	if (r->queue_compute) refer_free(r->queue_compute);
	if (r->device) refer_free(r->device);
}

iphyee_worker_s* iphyee_worker_alloc(iphyee_worker_instance_s *restrict instance, uintptr_t index)
{
	iphyee_worker_s *restrict r;
	if (instance && (r = (iphyee_worker_s *) refer_alloz(sizeof(iphyee_worker_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_free_func);
		if ((r->device = iphyee_worker_instance_create_device(instance, index)) &&
			(r->queue_compute = iphyee_worker_queue_create_compute(r->device, 0)) &&
			(r->queue_transfer = iphyee_worker_queue_create_transfer(r->device, 0)) &&
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
