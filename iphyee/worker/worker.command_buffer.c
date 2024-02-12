#include "worker.inner.h"

static void iphyee_worker_command_buffer_free_func(iphyee_worker_command_buffer_s *restrict r)
{
	if (r->command_buffer) vkFreeCommandBuffers(r->device, r->command_pool, 1, &r->command_buffer);
	if (r->depend) refer_free(r->depend);
}

static iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_alloc(iphyee_worker_command_pool_s *restrict command_pool, VkCommandBufferLevel level)
{
	iphyee_worker_command_buffer_s *restrict r;
	VkCommandBufferAllocateInfo info;
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = NULL;
	info.commandPool = command_pool->command_pool;
	info.level = level;
	info.commandBufferCount = 1;
	if ((r = (iphyee_worker_command_buffer_s *) refer_alloz(sizeof(iphyee_worker_command_buffer_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_command_buffer_free_func);
		r->depend = (iphyee_worker_command_pool_s *) refer_save(command_pool);
		r->device = command_pool->device;
		r->queue = command_pool->queue;
		r->command_pool = command_pool->command_pool;
		if (!vkAllocateCommandBuffers(r->device, &info, &r->command_buffer))
			return r;
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_create_primary(iphyee_worker_command_pool_s *restrict command_pool)
{
	return iphyee_worker_command_buffer_alloc(command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}

iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_create_secondary(iphyee_worker_command_pool_s *restrict command_pool)
{
	return iphyee_worker_command_buffer_alloc(command_pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY);
}

iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_reset(iphyee_worker_command_buffer_s *restrict r)
{
	if (!vkResetCommandBuffer(r->command_buffer, 0))
		return r;
	return NULL;
}

iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_begin(iphyee_worker_command_buffer_s *restrict r, uint32_t keep)
{
	VkCommandBufferBeginInfo info;
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = NULL;
	info.flags = keep?0:VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	info.pInheritanceInfo = NULL;
	if ((keep || !vkResetCommandBuffer(r->command_buffer, 0)) &&
		!vkBeginCommandBuffer(r->command_buffer, &info))
		return r;
	return NULL;
}

iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_end(iphyee_worker_command_buffer_s *restrict r)
{
	if (!vkEndCommandBuffer(r->command_buffer))
		return r;
	return NULL;
}

iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_submit(iphyee_worker_command_buffer_s *restrict r, iphyee_worker_fence_s *restrict fence, uintptr_t wait_count, iphyee_worker_semaphore_s **restrict wait_array, uintptr_t signal_count, iphyee_worker_semaphore_s **restrict signal_array)
{
	VkSubmitInfo info;
	VkSemaphore semaphore[wait_count + signal_count];
	VkPipelineStageFlags stage[wait_count];
	uintptr_t i;
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.pNext = NULL;
	info.waitSemaphoreCount = 0;
	info.pWaitSemaphores = NULL;
	info.pWaitDstStageMask = NULL;
	info.commandBufferCount = 1;
	info.pCommandBuffers = &r->command_buffer;
	info.signalSemaphoreCount = 0;
	info.pSignalSemaphores = NULL;
	if (wait_count)
	{
		for (i = 0; i < wait_count; ++i)
		{
			semaphore[i] = wait_array[i]->semaphore;
			stage[i] = wait_array[i]->wait_stage_mask;
		}
		info.waitSemaphoreCount = wait_count;
		info.pWaitSemaphores = semaphore;
		info.pWaitDstStageMask = stage;
	}
	if (signal_count)
	{
		for (i = 0; i < signal_count; ++i)
			semaphore[i] = signal_array[wait_count + i]->semaphore;
		info.signalSemaphoreCount = signal_count;
		info.pSignalSemaphores = semaphore + wait_count;
	}
	if (!vkQueueSubmit(r->queue, 1, &info, fence?fence->fence:NULL))
		return r;
	return NULL;
}
