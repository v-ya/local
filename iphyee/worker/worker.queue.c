#include "worker.inner.h"

static void iphyee_worker_queue_free_func(iphyee_worker_queue_s *restrict r)
{
	if (r->device) refer_free(r->device);
}

static iphyee_worker_queue_s* iphyee_worker_queue_alloc(iphyee_worker_device_s *restrict device, VkQueue queue)
{
	iphyee_worker_queue_s *restrict r;
	if ((r = (iphyee_worker_queue_s *) refer_alloz(sizeof(iphyee_worker_queue_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_queue_free_func);
		r->queue = queue;
		r->device = (iphyee_worker_device_s *) refer_save(device);
		return r;
	}
	return NULL;
}

iphyee_worker_queue_s* iphyee_worker_queue_create_compute(iphyee_worker_device_s *restrict device)
{
	return iphyee_worker_queue_alloc(device, device->queue_compute);
}

iphyee_worker_queue_s* iphyee_worker_queue_create_transfer(iphyee_worker_device_s *restrict device)
{
	return iphyee_worker_queue_alloc(device, device->queue_transfer);
}

iphyee_worker_queue_s* iphyee_worker_queue_wait_idle(iphyee_worker_queue_s *restrict r)
{
	if (!vkQueueWaitIdle(r->queue))
		return r;
	return NULL;
}
