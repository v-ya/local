#include "command_pri.h"
#include "type_pri.h"
#include "surface_pri.h"
#include "shader_pri.h"
#include "image_pri.h"
#include "buffer_pri.h"
#include <alloca.h>

static void graph_command_pool_free_func(register graph_command_pool_s *restrict r)
{
	register void *v;
	if ((v = r->cpool)) vkDestroyCommandPool(r->dev->dev, (VkCommandPool) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_command_pool_s* graph_command_pool_alloc(register struct graph_dev_s *restrict dev, register const struct graph_device_queue_t *restrict queue, graph_command_pool_flags_t flags, uint32_t number)
{
	register graph_command_pool_s *restrict r;
	VkCommandPoolCreateInfo info;
	VkCommandBufferAllocateInfo buffer;
	VkResult ret;
	if (!number) goto label_return_null;
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = NULL;
	info.flags = (VkCommandPoolCreateFlags) flags;
	info.queueFamilyIndex = (uint32_t) queue->index;
	r = (graph_command_pool_s *) refer_alloz(sizeof(graph_command_pool_s) + sizeof(VkCommandBuffer) * number);
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_command_pool_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		ret = vkCreateCommandPool(dev->dev, &info, &r->ga->alloc, &r->cpool);
		if (!ret)
		{
			r->clear[1].depthStencil.depth = 1.0f;
			r->clear_number = 1;
			r->primary_size = number;
			buffer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			buffer.pNext = NULL;
			buffer.commandPool = r->cpool;
			buffer.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			buffer.commandBufferCount = number;
			ret = vkAllocateCommandBuffers(dev->dev, &buffer, r->primary);
			if (!ret) return r;
			mlog_printf(r->ml, "[graph_command_pool_alloc] vkAllocateCommandBuffers = %d\n", ret);
		}
		else mlog_printf(r->ml, "[graph_command_pool_alloc] vkCreateCommandPool = %d\n", ret);
		refer_free(r);
	}
	label_return_null:
	return NULL;
}

graph_command_pool_s* graph_command_pool_begin(register graph_command_pool_s *restrict r, uint32_t ia, graph_command_buffer_usage_t usage)
{
	VkCommandBufferBeginInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = NULL;
	info.flags = (VkCommandBufferUsageFlags) usage;
	// 只用于辅助指令缓冲
	info.pInheritanceInfo = NULL;
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
		{
			ret = vkBeginCommandBuffer(r->primary[ia], &info);
			if (ret) goto label_fail;
		}
	}
	else if (ia < r->primary_size)
	{
		ret = vkBeginCommandBuffer(r->primary[ia], &info);
		if (ret)
		{
			label_fail:
			mlog_printf(r->ml, "[graph_command_pool_begin] vkBeginCommandBuffer [%u] = %u\n", ia);
			r = NULL;
		}
	}
	else r = NULL;
	return r;
}

graph_command_pool_s* graph_command_pool_end(register graph_command_pool_s *restrict r, uint32_t ia)
{
	VkResult ret;
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
		{
			ret = vkEndCommandBuffer(r->primary[ia]);
			if (ret) goto label_fail;
		}
	}
	else if (ia < r->primary_size)
	{
		ret = vkEndCommandBuffer(r->primary[ia]);
		if (ret)
		{
			label_fail:
			mlog_printf(r->ml, "[graph_command_pool_begin] vkEndCommandBuffer [%u] = %u\n", ia);
			r = NULL;
		}
	}
	else r = NULL;
	return r;
}

void graph_command_begin_render(register graph_command_pool_s *restrict r, uint32_t ia, const struct graph_render_pass_s *restrict render, struct graph_frame_buffer_s *restrict frame, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
	VkRenderPassBeginInfo info;
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.pNext = NULL;
	info.renderPass = render->render;
	info.framebuffer = frame->frame_buffer;
	info.renderArea.offset.x = x;
	info.renderArea.offset.y = y;
	info.renderArea.extent.width = width;
	info.renderArea.extent.height = height;
	info.clearValueCount = 2;
	info.pClearValues = r->clear;
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdBeginRenderPass(r->primary[ia], &info, VK_SUBPASS_CONTENTS_INLINE);
	}
	else if (ia < r->primary_size)
		vkCmdBeginRenderPass(r->primary[ia], &info, VK_SUBPASS_CONTENTS_INLINE);
}

void graph_command_bind_pipe(register graph_command_pool_s *restrict r, uint32_t ia, graph_pipeline_bind_point_t type, const struct graph_pipe_s *restrict pipe)
{
	VkPipelineBindPoint tp;
	tp = graph_pipeline_bind_point2vk(type);
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdBindPipeline(r->primary[ia], tp, pipe->pipe);
	}
	else if (ia < r->primary_size)
		vkCmdBindPipeline(r->primary[ia], tp, pipe->pipe);
}

graph_command_pool_s* graph_command_bind_vertex_buffers(register graph_command_pool_s *restrict r, uint32_t ia, uint32_t first_binding, uint32_t n, const struct graph_buffer_s *const restrict *restrict buffers, const uint64_t *restrict offsets)
{
	VkBuffer *b;
	VkDeviceSize *o;
	if (n)
	{
		b = (VkBuffer *) alloca(sizeof(VkBuffer) * n);
		o = (VkDeviceSize *) alloca(sizeof(VkDeviceSize) * n);
		if (b && o)
		{
			register uint32_t i;
			for (i = 0; i < n; ++i)
			{
				b[i] = buffers[i]->buffer;
				o[i] = offsets[i];
			}
			if (!~ia)
			{
				for (ia = 0; ia < r->primary_size; ++ia)
					vkCmdBindVertexBuffers(r->primary[ia], first_binding, n, b, o);
			}
			else if (ia < r->primary_size)
				vkCmdBindVertexBuffers(r->primary[ia], first_binding, n, b, o);
			return r;
		
		}
	}
	return NULL;
}

void graph_command_draw(register graph_command_pool_s *restrict r, uint32_t ia, uint32_t v_number, uint32_t i_number, uint32_t v_start, uint32_t i_start)
{
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdDraw(r->primary[ia], v_number, i_number, v_start, i_start);
	}
	else if (ia < r->primary_size)
		vkCmdDraw(r->primary[ia], v_number, i_number, v_start, i_start);
}

void graph_command_end_render(register graph_command_pool_s *restrict r, uint32_t ia)
{
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdEndRenderPass(r->primary[ia]);
	}
	else if (ia < r->primary_size)
		vkCmdEndRenderPass(r->primary[ia]);
}

struct graph_queue_t* graph_queue_submit(struct graph_queue_t *restrict queue, graph_command_pool_s *restrict pool, uint32_t index, graph_semaphore_s *restrict wait, graph_semaphore_s *restrict signal, graph_fence_s *restrict fence, graph_pipeline_stage_flags_t wait_mask)
{
	VkSubmitInfo info;
	VkResult ret;
	if (index < pool->primary_size)
	{
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext = NULL;
		info.waitSemaphoreCount = !!wait;
		info.pWaitSemaphores = wait?&wait->semaphore:NULL;
		info.pWaitDstStageMask = (VkPipelineStageFlags *) &wait_mask;
		info.commandBufferCount = 1;
		info.pCommandBuffers = pool->primary + index;
		info.signalSemaphoreCount = !!signal;
		info.pSignalSemaphores = signal?&signal->semaphore:NULL;
		ret = vkQueueSubmit(queue->queue, 1, &info, fence?fence->fence:NULL);
		if (!ret) return queue;
		mlog_printf(pool->ml, "[graph_queue_submit] vkQueueSubmit = %d\n", ret);
	}
	return NULL;
}

struct graph_queue_t* graph_queue_present(struct graph_queue_t *restrict queue, struct graph_swapchain_s *restrict swapchain, uint32_t index, graph_semaphore_s *restrict wait)
{
	VkPresentInfoKHR info;
	VkResult ret;
	if (index < swapchain->image_number)
	{
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.pNext = NULL;
		info.waitSemaphoreCount = !!wait;
		info.pWaitSemaphores = wait?&wait->semaphore:NULL;
		info.swapchainCount = 1;
		info.pSwapchains = &swapchain->swapchain;
		info.pImageIndices = &index;
		info.pResults = NULL;
		ret = vkQueuePresentKHR(queue->queue, &info);
		if (!ret) return queue;
		mlog_printf(swapchain->ml, "[graph_queue_present] vkQueuePresentKHR = %d\n", ret);
	}
	return NULL;
}

struct graph_queue_t* graph_queue_wait_idle(struct graph_queue_t *restrict queue)
{
	VkResult ret;
	ret = vkQueueWaitIdle(queue->queue);
	if (!ret) return queue;
	return NULL;
}

static void graph_semaphore_free_func(register graph_semaphore_s *restrict r)
{
	register void *v;
	if ((v = r->semaphore)) vkDestroySemaphore(r->dev->dev, (VkSemaphore) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_semaphore_s* graph_semaphore_alloc(register struct graph_dev_s *restrict dev)
{
	register graph_semaphore_s *restrict r;
	VkSemaphoreCreateInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	r = (graph_semaphore_s *) refer_alloz(sizeof(graph_semaphore_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_semaphore_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		ret = vkCreateSemaphore(dev->dev, &info, &r->ga->alloc, &r->semaphore);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_semaphore_alloc] vkCreateSemaphore = %d\n", ret);
		refer_free(r);
	}
	return NULL;
}

graph_semaphore_s* graph_semaphore_signal(register graph_semaphore_s *restrict semaphore, uint64_t value)
{
	VkSemaphoreSignalInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
	info.pNext = NULL;
	info.semaphore = semaphore->semaphore;
	info.value = value;
	ret = vkSignalSemaphore(semaphore->dev->dev, &info);
	if (!ret) return semaphore;
	mlog_printf(semaphore->ml, "[graph_semaphore_signal] vkSignalSemaphore = %d\n", ret);
	return NULL;
}

graph_semaphore_s* graph_semaphore_wait(register graph_semaphore_s *restrict semaphore, uint64_t timeout, uint64_t value)
{
	VkSemaphoreWaitInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	info.pNext = NULL;
	info.flags = VK_SEMAPHORE_WAIT_ANY_BIT;
	info.semaphoreCount = 1;
	info.pSemaphores = &semaphore->semaphore;
	info.pValues = &value;
	ret = vkWaitSemaphores(semaphore->dev->dev, &info, timeout);
	if (!ret) return semaphore;
	mlog_printf(semaphore->ml, "[graph_semaphore_wait] vkWaitSemaphores = %d\n", ret);
	return NULL;
}

static void graph_fence_free_func(register graph_fence_s *restrict r)
{
	register void *v;
	if ((v = r->fence)) vkDestroyFence(r->dev->dev, (VkFence) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_fence_s* graph_fence_alloc(register struct graph_dev_s *restrict dev, graph_fence_flags_t flags)
{
	register graph_fence_s *restrict r;
	VkFenceCreateInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.pNext = NULL;
	info.flags = (VkFenceCreateFlags) flags;
	r = (graph_fence_s *) refer_alloz(sizeof(graph_semaphore_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_fence_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		ret = vkCreateFence(dev->dev, &info, &r->ga->alloc, &r->fence);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_fence_alloc] vkCreateFence = %d\n", ret);
		refer_free(r);
	}
	return NULL;
}

graph_fence_s* graph_fence_reset(register graph_fence_s *restrict fence)
{
	VkResult ret;
	ret = vkResetFences(fence->dev->dev, 1, &fence->fence);
	if (!ret) return fence;
	mlog_printf(fence->ml, "[graph_fence_reset] vkResetFences = %d\n", ret);
	return NULL;
}

graph_fence_s* graph_fence_wait(register graph_fence_s *restrict fence, uint64_t timeout)
{
	VkResult ret;
	ret = vkWaitForFences(fence->dev->dev, 1, &fence->fence, VK_FALSE, timeout);
	if (!ret) return fence;
	mlog_printf(fence->ml, "[graph_fence_reset] vkResetFences = %d\n", ret);
	return NULL;
}
