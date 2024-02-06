#include "command_pri.h"
#include "type_pri.h"
#include "surface_pri.h"
#include "shader_pri.h"
#include "image_pri.h"
#include "buffer_pri.h"
#include "layout_pri.h"
#include <alloca.h>

static void graph_command_pool_free_func(graph_command_pool_s *restrict r)
{
	void *v;
	if ((v = r->cpool)) vkDestroyCommandPool(r->dev->dev, (VkCommandPool) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_command_pool_s* graph_command_pool_alloc(struct graph_dev_s *restrict dev, const struct graph_device_queue_t *restrict queue, graph_command_pool_flags_t flags, uint32_t number)
{
	graph_command_pool_s *restrict r;
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

graph_command_pool_s* graph_command_pool_begin(graph_command_pool_s *restrict r, uint32_t ia, graph_command_buffer_usage_t usage)
{
	VkCommandBufferBeginInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = NULL;
	info.flags = (VkCommandBufferUsageFlags) usage;
	info.pInheritanceInfo = NULL;
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

graph_command_pool_s* graph_command_pool_end(graph_command_pool_s *restrict r, uint32_t ia)
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

void graph_command_begin_render(graph_command_pool_s *restrict r, uint32_t ia, const struct graph_render_pass_s *restrict render, struct graph_frame_buffer_s *restrict frame, int32_t x, int32_t y, uint32_t width, uint32_t height)
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

void graph_command_bind_pipe(graph_command_pool_s *restrict r, uint32_t ia, graph_pipeline_bind_point_t type, const struct graph_pipe_s *restrict pipe)
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

void graph_command_set_viewport(graph_command_pool_s *restrict r, uint32_t ia, const struct graph_viewports_scissors_s *restrict vs)
{
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdSetViewport(r->primary[ia], 0, vs->viewport_number, vs->viewports);
	}
	else if (ia < r->primary_size)
		vkCmdSetViewport(r->primary[ia], 0, vs->viewport_number, vs->viewports);
}

void graph_command_set_scissor(graph_command_pool_s *restrict r, uint32_t ia, const struct graph_viewports_scissors_s *restrict vs)
{
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdSetScissor(r->primary[ia], 0, vs->scissor_number, vs->scissors);
	}
	else if (ia < r->primary_size)
		vkCmdSetScissor(r->primary[ia], 0, vs->scissor_number, vs->scissors);
}

graph_command_pool_s* graph_command_bind_vertex_buffers(graph_command_pool_s *restrict r, uint32_t ia, uint32_t first_binding, uint32_t n, const struct graph_buffer_s *const restrict *restrict buffers, const uint64_t *restrict offsets)
{
	VkBuffer *b;
	VkDeviceSize *o;
	if (n)
	{
		b = (VkBuffer *) alloca(sizeof(VkBuffer) * n);
		o = (VkDeviceSize *) alloca(sizeof(VkDeviceSize) * n);
		if (b && o)
		{
			uint32_t i;
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

graph_command_pool_s* graph_command_bind_index_buffer(graph_command_pool_s *restrict r, uint32_t ia, const struct graph_buffer_s *restrict buffer, uint64_t offset, graph_index_type_t type)
{
	VkBuffer b;
	VkIndexType t;
	if ((uint32_t) type < graph_index_type$number)
	{
		b = buffer->buffer;
		t = graph_index_type2vk(type);
		if (!~ia)
		{
			for (ia = 0; ia < r->primary_size; ++ia)
				vkCmdBindIndexBuffer(r->primary[ia], b, offset, t);
		}
		else if (ia < r->primary_size)
			vkCmdBindIndexBuffer(r->primary[ia], b, offset, t);
		return r;
	}
	return NULL;
}

graph_command_pool_s* graph_command_bind_desc_sets(graph_command_pool_s *restrict r, uint32_t ia, graph_pipeline_bind_point_t point, const struct graph_pipe_layout_s *restrict layout, const struct graph_descriptor_sets_s *restrict sets, uint32_t set_start, uint32_t set_number, uint32_t dy_offset_number, const uint32_t *restrict dy_offset)
{
	if ((uint32_t) point < graph_pipeline_bind_point$number && set_start + set_number <= sets->number)
	{
		const VkDescriptorSet *s;
		VkPipelineBindPoint bp;
		VkPipelineLayout ly;
		bp = graph_pipeline_bind_point2vk(point);
		ly = layout->layout;
		s = sets->set + set_start;
		if (!~ia)
		{
			for (ia = 0; ia < r->primary_size; ++ia)
				vkCmdBindDescriptorSets(r->primary[ia], bp, ly, 0, set_number, s, dy_offset_number, dy_offset);
		}
		else if (ia < r->primary_size)
			vkCmdBindDescriptorSets(r->primary[ia], bp, ly, 0, set_number, s, dy_offset_number, dy_offset);
		return r;
	}
	return NULL;
}

void graph_command_draw(graph_command_pool_s *restrict r, uint32_t ia, uint32_t v_number, uint32_t i_number, uint32_t v_start, uint32_t i_start)
{
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdDraw(r->primary[ia], v_number, i_number, v_start, i_start);
	}
	else if (ia < r->primary_size)
		vkCmdDraw(r->primary[ia], v_number, i_number, v_start, i_start);
}

void graph_command_draw_index(graph_command_pool_s *restrict r, uint32_t ia, uint32_t idx_number, uint32_t i_number, uint32_t idx_start, uint32_t v_start, uint32_t i_start)
{
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdDrawIndexed(r->primary[ia], idx_number, i_number, idx_start, v_start, i_start);
	}
	else if (ia < r->primary_size)
		vkCmdDrawIndexed(r->primary[ia], idx_number, i_number, idx_start, v_start, i_start);
}

void graph_command_end_render(graph_command_pool_s *restrict r, uint32_t ia)
{
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdEndRenderPass(r->primary[ia]);
	}
	else if (ia < r->primary_size)
		vkCmdEndRenderPass(r->primary[ia]);
}

void graph_command_dispatch(graph_command_pool_s *restrict r, uint32_t ia, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
{
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdDispatch(r->primary[ia], group_count_x, group_count_y, group_count_z);
	}
	else if (ia < r->primary_size)
		vkCmdDispatch(r->primary[ia], group_count_x, group_count_y, group_count_z);
}

void graph_command_copy_buffer(graph_command_pool_s *restrict r, uint32_t ia, struct graph_buffer_s *restrict dst, const struct graph_buffer_s *restrict src, uint64_t dst_offset, uint64_t src_offset, uint64_t size)
{
	VkBuffer s, d;
	VkBufferCopy copy;
	copy.srcOffset = src_offset;
	copy.dstOffset = dst_offset;
	copy.size = size;
	s = src->buffer;
	d = dst->buffer;
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdCopyBuffer(r->primary[ia], s, d, 1, &copy);
	}
	else if (ia < r->primary_size)
		vkCmdCopyBuffer(r->primary[ia], s, d, 1, &copy);
}

graph_command_pool_s* graph_command_copy_buffer_to_image(graph_command_pool_s *restrict r, uint32_t ia, struct graph_image_s *restrict dst, const struct graph_buffer_s *restrict src, graph_image_layout_t layout, uint64_t buffer_offset, graph_image_aspect_flags_t flags, const int32_t *restrict image_offset, const uint32_t *restrict image_extent)
{
	VkBuffer s;
	VkImage d;
	VkBufferImageCopy copy;
	VkImageLayout ly;
	if ((uint32_t) layout < graph_image_layout$number)
	{
		copy.bufferOffset = buffer_offset;
		copy.bufferRowLength = 0;
		copy.bufferImageHeight = 0;
		copy.imageSubresource.aspectMask = (VkImageAspectFlags) flags;
		copy.imageSubresource.mipLevel = 0;
		copy.imageSubresource.baseArrayLayer = 0;
		copy.imageSubresource.layerCount = 1;
		copy.imageOffset.x = 0;
		copy.imageOffset.y = 0;
		copy.imageOffset.z = 0;
		copy.imageExtent = dst->extent;
		if (image_offset) switch (dst->type)
		{
			case VK_IMAGE_TYPE_3D:
				copy.imageOffset.z = image_offset[2];
				// fall through
			case VK_IMAGE_TYPE_2D:
				copy.imageOffset.y = image_offset[1];
				// fall through
			case VK_IMAGE_TYPE_1D:
				copy.imageOffset.x = image_offset[0];
				// fall through
			default:
				break;
		}
		if (image_extent) switch (dst->type)
		{
			case VK_IMAGE_TYPE_3D:
				copy.imageExtent.depth = image_extent[2];
				// fall through
			case VK_IMAGE_TYPE_2D:
				copy.imageExtent.height = image_extent[1];
				// fall through
			case VK_IMAGE_TYPE_1D:
				copy.imageExtent.width = image_extent[0];
				// fall through
			default:
				break;
		}
		s = src->buffer;
		d = dst->image;
		ly = graph_image_layout2vk(layout);
		if (!~ia)
		{
			for (ia = 0; ia < r->primary_size; ++ia)
				vkCmdCopyBufferToImage(r->primary[ia], s, d, ly, 1, &copy);
		}
		else if (ia < r->primary_size)
			vkCmdCopyBufferToImage(r->primary[ia], s, d, ly, 1, &copy);
	}
	return NULL;
}

graph_command_pipe_barrier_param_s* graph_command_pipe_barrier_param_alloc(uint32_t memory_number, uint32_t buffer_number, uint32_t image_number)
{
	graph_command_pipe_barrier_param_s *restrict r;
	size_t n;
	n = sizeof(graph_command_pipe_barrier_param_s) +
		sizeof(VkMemoryBarrier) * memory_number + 
		sizeof(VkBufferMemoryBarrier) * buffer_number +
		sizeof(VkImageMemoryBarrier) * image_number;
	r = (graph_command_pipe_barrier_param_s *) refer_alloz(n);
	if (r)
	{
		r->memory_size = memory_number;
		r->memory = (VkMemoryBarrier *) (r + 1);
		r->buffer_size = buffer_number;
		r->buffer = (VkBufferMemoryBarrier *) (r->memory + memory_number);
		r->image_size = image_number;
		r->image = (VkImageMemoryBarrier *) (r->buffer + buffer_number);
		while (memory_number)
			r->memory[--memory_number].sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		while (buffer_number)
			r->buffer[--buffer_number].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		while (image_number)
		{
			r->image[--image_number].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			r->image[image_number].srcQueueFamilyIndex =
				r->image[image_number].dstQueueFamilyIndex =
				VK_QUEUE_FAMILY_IGNORED;
		}
	}
	return r;
}

void graph_command_pipe_barrier_param_set_image(graph_command_pipe_barrier_param_s *restrict param, uint32_t index, const struct graph_image_s *restrict image, graph_image_aspect_flags_t flags, uint32_t mip_level_base, uint32_t mip_level_number, uint32_t layer_base, uint32_t layer_number)
{
	if (index < param->image_size)
	{
		VkImageMemoryBarrier *restrict p;
		p = param->image + index;
		p->image = image->image;
		p->subresourceRange.aspectMask = (VkImageAspectFlags) flags;
		p->subresourceRange.baseMipLevel = mip_level_base;
		p->subresourceRange.levelCount = mip_level_number;
		p->subresourceRange.baseArrayLayer = layer_base;
		p->subresourceRange.layerCount = layer_number;
		if (index >= param->image_number)
			param->image_number = index + 1;
	}
}

void graph_command_pipe_barrier_param_set_image_access(graph_command_pipe_barrier_param_s *restrict param, uint32_t index, graph_access_flags_t src, graph_access_flags_t dst)
{
	if (index < param->image_size)
	{
		VkImageMemoryBarrier *restrict p;
		p = param->image + index;
		p->srcAccessMask = (VkAccessFlags) src;
		p->dstAccessMask = (VkAccessFlags) dst;
	}
}

void graph_command_pipe_barrier_param_set_image_layout(graph_command_pipe_barrier_param_s *restrict param, uint32_t index, graph_image_layout_t old, graph_image_layout_t new)
{
	if (index < param->image_size)
	{
		VkImageMemoryBarrier *restrict p;
		p = param->image + index;
		p->oldLayout = (VkImageLayout) old;
		p->newLayout = (VkImageLayout) new;
	}
}

void graph_command_pipe_barrier_param_set_image_queue(graph_command_pipe_barrier_param_s *restrict param, uint32_t index, const struct graph_device_queue_t *restrict src, const struct graph_device_queue_t *restrict dst)
{
	if (index < param->image_size)
	{
		VkImageMemoryBarrier *restrict p;
		p = param->image + index;
		if (src) p->srcQueueFamilyIndex = (uint32_t) src->index;
		else p->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		if (dst) p->dstQueueFamilyIndex = (uint32_t) dst->index;
		else p->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	}
}

void graph_command_pipe_barrier(graph_command_pool_s *restrict r, uint32_t ia, graph_pipeline_stage_flags_t src, graph_pipeline_stage_flags_t dst, graph_dependency_flags_t flags, const graph_command_pipe_barrier_param_s *restrict param)
{
	const VkMemoryBarrier *memory;
	const VkBufferMemoryBarrier *buffer;
	const VkImageMemoryBarrier *image;
	uint32_t n_memory, n_buffer, n_image;
	n_memory = n_buffer = n_image = 0;
	memory = NULL;
	buffer = NULL;
	image = NULL;
	if (param)
	{
		if ((n_memory = param->memory_number))
			memory = param->memory;
		if ((n_buffer = param->buffer_number))
			buffer = param->buffer;
		if ((n_image = param->image_number))
			image = param->image;
	}
	if (!~ia)
	{
		for (ia = 0; ia < r->primary_size; ++ia)
			vkCmdPipelineBarrier(r->primary[ia],
				(VkPipelineStageFlags) src,
				(VkPipelineStageFlags) dst,
				(VkDependencyFlags) flags,
				n_memory, memory,
				n_buffer, buffer,
				n_image, image);
	}
	else if (ia < r->primary_size)
		vkCmdPipelineBarrier(r->primary[ia],
			(VkPipelineStageFlags) src,
			(VkPipelineStageFlags) dst,
			(VkDependencyFlags) flags,
			n_memory, memory,
			n_buffer, buffer,
			n_image, image);
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

static void graph_semaphore_free_func(graph_semaphore_s *restrict r)
{
	void *v;
	if ((v = r->semaphore)) vkDestroySemaphore(r->dev->dev, (VkSemaphore) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_semaphore_s* graph_semaphore_alloc(struct graph_dev_s *restrict dev)
{
	graph_semaphore_s *restrict r;
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

graph_semaphore_s* graph_semaphore_signal(graph_semaphore_s *restrict semaphore, uint64_t value)
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

graph_semaphore_s* graph_semaphore_wait(graph_semaphore_s *restrict semaphore, uint64_t timeout, uint64_t value)
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

static void graph_fence_free_func(graph_fence_s *restrict r)
{
	void *v;
	if ((v = r->fence)) vkDestroyFence(r->dev->dev, (VkFence) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_fence_s* graph_fence_alloc(struct graph_dev_s *restrict dev, graph_fence_flags_t flags)
{
	graph_fence_s *restrict r;
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

graph_fence_s* graph_fence_reset(graph_fence_s *restrict fence)
{
	VkResult ret;
	ret = vkResetFences(fence->dev->dev, 1, &fence->fence);
	if (!ret) return fence;
	mlog_printf(fence->ml, "[graph_fence_reset] vkResetFences = %d\n", ret);
	return NULL;
}

graph_fence_s* graph_fence_wait(graph_fence_s *restrict fence, uint64_t timeout)
{
	VkResult ret;
	ret = vkWaitForFences(fence->dev->dev, 1, &fence->fence, VK_FALSE, timeout);
	if (!ret) return fence;
	mlog_printf(fence->ml, "[graph_fence_reset] vkResetFences = %d\n", ret);
	return NULL;
}
