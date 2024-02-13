#include "worker.inner.h"

static void iphyee_worker_queue_free_func(iphyee_worker_queue_s *restrict r)
{
	if (r->device) refer_free(r->device);
}

static iphyee_worker_queue_s* iphyee_worker_queue_alloc(iphyee_worker_device_s *restrict device, iphyee_worker_device_queue_t *restrict queue)
{
	iphyee_worker_queue_s *restrict r;
	if (queue->queue_uconut < queue->queue_number &&
		(r = (iphyee_worker_queue_s *) refer_alloz(sizeof(iphyee_worker_queue_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_queue_free_func);
		r->device = (iphyee_worker_device_s *) refer_save(device);
		vkGetDeviceQueue(device->device, queue->family_index, queue->queue_offset + queue->queue_uconut, &r->queue);
		if (r->queue)
		{
			r->family_index = queue->family_index;
			r->queue_index = queue->queue_offset + queue->queue_uconut;
			queue->queue_uconut += 1;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_queue_s* iphyee_worker_queue_create_compute(iphyee_worker_device_s *restrict device)
{
	return iphyee_worker_queue_alloc(device, &device->queue_compute);
}

iphyee_worker_queue_s* iphyee_worker_queue_create_transfer(iphyee_worker_device_s *restrict device)
{
	return iphyee_worker_queue_alloc(device, &device->queue_transfer);
}

iphyee_worker_queue_s* iphyee_worker_queue_wait_idle(iphyee_worker_queue_s *restrict r)
{
	if (!vkQueueWaitIdle(r->queue))
		return r;
	return NULL;
}
