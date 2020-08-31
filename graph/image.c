#include "image_pri.h"
#include "type_pri.h"
#include <memory.h>

graph_image_view_param_s* graph_image_view_param_alloc(graph_image_view_type_t type)
{
	register graph_image_view_param_s *restrict r;
	r = NULL;
	if ((uint32_t) type < graph_image_view_type$number)
	{
		r = (graph_image_view_param_s *) refer_alloc(sizeof(graph_image_view_param_s));
		if (r)
		{
			r->flags = 0;
			r->type = graph_image_view_type2vk(type);
			r->format = VK_FORMAT_UNDEFINED;
			r->components = (const VkComponentMapping) {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			};
			r->range = (const VkImageSubresourceRange) {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			};
		}
	}
	return r;
}

static void graph_image_view_free_func(register graph_image_view_s *restrict r)
{
	register refer_t v;
	if ((v = r->view))
		vkDestroyImageView(r->dev->dev, (VkImageView) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->depend)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_image_view_s* graph_image_view_alloc_by_swapchain(register const graph_image_view_param_s *restrict param, register struct graph_swapchain_s *restrict swapchain, uint32_t index)
{
	register graph_image_view_s *restrict r;
	VkImageViewCreateInfo info;
	VkResult ret;
	if (index < swapchain->image_number)
	{
		r = (graph_image_view_s *) refer_alloz(sizeof(graph_image_view_s));
		if (r)
		{
			refer_set_free(r, (refer_free_f) graph_image_view_free_func);
			r->ml = (mlog_s *) refer_save(swapchain->ml);
			r->depend = refer_save(swapchain);
			r->dev = (graph_dev_s *) refer_save(swapchain->dev);
			r->image = swapchain->image_array[index];
			r->ga = (graph_allocator_s *) refer_save(swapchain->ga);
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.pNext = NULL;
			info.flags = param->flags;
			info.image = r->image;
			info.viewType = param->type;
			info.format = param->format?param->format:swapchain->info->imageFormat;
			info.components = param->components;
			info.subresourceRange = param->range;
			ret = vkCreateImageView(swapchain->dev->dev, &info, &r->ga->alloc, &r->view);
			if (!ret) return r;
			mlog_printf(r->ml, "[graph_image_view_alloc_by_swapchain] vkCreateImageView = %d\n", ret);
			refer_free(r);
		}
	}
	return NULL;
}

static void graph_frame_buffer_free_func(register graph_frame_buffer_s *restrict r)
{
	register void *v;
	if ((v = r->frame_buffer)) vkDestroyFramebuffer(r->dev->dev, (VkFramebuffer) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->render)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_frame_buffer_s* graph_frame_buffer_alloc(register struct graph_render_pass_s *restrict render, register graph_image_view_s *restrict view, uint32_t width, uint32_t height, uint32_t layers)
{
	register graph_frame_buffer_s *restrict r;
	VkFramebufferCreateInfo info;
	VkImageView iv;
	VkResult ret;
	if (width && height && layers)
	{
		iv = view->view;
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.pNext = NULL;
		info.flags = 0;
		info.renderPass = render->render;
		info.attachmentCount = 1;
		info.pAttachments = &iv;
		info.width = width;
		info.height = height;
		info.layers = layers;
		r = (graph_frame_buffer_s *) refer_alloz(sizeof(graph_frame_buffer_s));
		if (r)
		{
			refer_set_free(r, (refer_free_f) graph_frame_buffer_free_func);
			r->ml = (mlog_s *) refer_save(render->ml);
			r->dev = (graph_dev_s *) refer_save(render->dev);
			r->render = (graph_render_pass_s *) refer_save(render);
			r->ga = (graph_allocator_s *) refer_save(render->ga);
			ret = vkCreateFramebuffer(r->dev->dev, &info, &r->ga->alloc, &r->frame_buffer);
			if (!ret) return r;
			mlog_printf(r->ml, "[graph_frame_buffer_alloc] vkCreateFramebuffer = %d\n", ret);
			refer_free(r);
		}
	}
	return NULL;
}
