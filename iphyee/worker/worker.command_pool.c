#include "worker.inner.h"

static void iphyee_worker_command_pool_free_func(iphyee_worker_command_pool_s *restrict r)
{
	if (r->command_pool) vkDestroyCommandPool(r->device, r->command_pool, NULL);
	if (r->depend) refer_free(r->depend);
}

iphyee_worker_command_pool_s* iphyee_worker_command_pool_alloc(iphyee_worker_queue_s *restrict queue, uint32_t short_live)
{
	iphyee_worker_command_pool_s *restrict r;
	VkCommandPoolCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = NULL;
	info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	info.queueFamilyIndex = queue->family_index;
	if (short_live) info.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	if ((r = (iphyee_worker_command_pool_s *) refer_alloz(sizeof(iphyee_worker_command_pool_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_command_pool_free_func);
		r->depend = (iphyee_worker_queue_s *) refer_save(queue);
		r->device = queue->device->device;
		r->queue = queue->queue;
		if (!vkCreateCommandPool(r->device, &info, NULL, &r->command_pool))
			return r;
		refer_free(r);
	}
	return NULL;
}
