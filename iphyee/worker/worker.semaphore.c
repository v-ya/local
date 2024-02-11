#include "worker.inner.h"

static void iphyee_worker_semaphore_free_func(iphyee_worker_semaphore_s *restrict r)
{
	if (r->semaphore) vkDestroySemaphore(r->device, r->semaphore, NULL);
	if (r->depend) refer_free(r->depend);
}

iphyee_worker_semaphore_s* iphyee_worker_semaphore_alloc(iphyee_worker_device_s *restrict device)
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
		if (!vkCreateSemaphore(r->device, &info, NULL, &r->semaphore))
			return r;
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_semaphore_s* iphyee_worker_semaphore_signal(iphyee_worker_semaphore_s *restrict semaphore, uint64_t value)
{
	VkSemaphoreSignalInfo info;
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
	info.pNext = NULL;
	info.semaphore = semaphore->semaphore;
	info.value = value;
	if (!vkSignalSemaphore(semaphore->device, &info))
		return semaphore;
	return NULL;
}

iphyee_worker_semaphore_s* iphyee_worker_semaphore_wait(iphyee_worker_semaphore_s *restrict semaphore, uint64_t timeout, uint64_t value)
{
	VkSemaphoreWaitInfo info;
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	info.pNext = NULL;
	info.flags = VK_SEMAPHORE_WAIT_ANY_BIT;
	info.semaphoreCount = 1;
	info.pSemaphores = &semaphore->semaphore;
	info.pValues = &value;
	if (!vkWaitSemaphores(semaphore->device, &info, timeout))
		return semaphore;
	return NULL;
}
