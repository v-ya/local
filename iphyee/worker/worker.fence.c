#include "worker.inner.h"

static void iphyee_worker_fence_free_func(iphyee_worker_fence_s *restrict r)
{
	if (r->fence) vkDestroyFence(r->device, r->fence, NULL);
	if (r->depend) refer_free(r->depend);
}

iphyee_worker_fence_s* iphyee_worker_fence_alloc(iphyee_worker_device_s *restrict device, uint32_t signaled)
{
	iphyee_worker_fence_s *restrict r;
	VkFenceCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.pNext = NULL;
	info.flags = signaled?VK_FENCE_CREATE_SIGNALED_BIT:0;
	if ((r = (iphyee_worker_fence_s *) refer_alloz(sizeof(iphyee_worker_fence_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_fence_free_func);
		r->depend = (iphyee_worker_device_s *) refer_save(device);
		r->device = device->device;
		if (!vkCreateFence(r->device, &info, NULL, &r->fence))
			return r;
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_fence_s* iphyee_worker_fence_reset(iphyee_worker_fence_s *restrict fence)
{
	if (!vkResetFences(fence->device, 1, &fence->fence))
		return fence;
	return NULL;
}

iphyee_worker_fence_s* iphyee_worker_fence_is_signaled(iphyee_worker_fence_s *restrict fence)
{
	if (!vkGetFenceStatus(fence->device, fence->fence))
		return fence;
	return NULL;
}

iphyee_worker_fence_s* iphyee_worker_fence_wait(iphyee_worker_fence_s *restrict fence, uint64_t timeout)
{
	if (!vkWaitForFences(fence->device, 1, &fence->fence, VK_FALSE, timeout))
		return fence;
	return NULL;
}
