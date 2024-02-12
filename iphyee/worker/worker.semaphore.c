#include "worker.inner.h"

static void iphyee_worker_semaphore_free_func(iphyee_worker_semaphore_s *restrict r)
{
	if (r->semaphore) vkDestroySemaphore(r->device, r->semaphore, NULL);
	if (r->depend) refer_free(r->depend);
}

iphyee_worker_semaphore_s* iphyee_worker_semaphore_alloc(iphyee_worker_device_s *restrict device, uint32_t stage_compute, uint32_t stage_transfer)
{
	iphyee_worker_semaphore_s *restrict r;
	VkSemaphoreCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	if ((r = (iphyee_worker_semaphore_s *) refer_alloz(sizeof(iphyee_worker_semaphore_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_semaphore_free_func);
		r->depend = (iphyee_worker_device_s *) refer_save(device);
		r->device = device->device;
		r->wait_stage_mask = 0;
		if (stage_compute) r->wait_stage_mask |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		if (stage_transfer) r->wait_stage_mask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
		if (!vkCreateSemaphore(r->device, &info, NULL, &r->semaphore))
			return r;
		refer_free(r);
	}
	return NULL;
}
