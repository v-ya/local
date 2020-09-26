#include "image_pri.h"
#include "buffer_pri.h"
#include "type_pri.h"
#include <memory.h>

static void graph_image_info_init_data(VkImageCreateInfo *restrict info, const graph_image_param_s *restrict param)
{
	static VkImageCreateInfo s_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_MAX_ENUM,
		.format = VK_FORMAT_UNDEFINED,
		.extent = {1, 1, 1},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	};
	if (!param) memcpy(info, &s_info, sizeof(s_info));
	else memcpy(info, &param->info, sizeof(param->info));
}

static void graph_image_free_func(register graph_image_s *restrict r)
{
	register refer_t v;
	if ((v = r->image))
		vkDestroyImage(r->dev->dev, (VkImage) v, &r->ga->alloc);
	if ((v = r->memory))
		vkFreeMemory(r->dev->dev, (VkDeviceMemory) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
	if ((v = r->heap)) refer_free(v);
}

static graph_image_s* graph_image_alloc(register struct graph_memory_heap_s *restrict heap, const VkImageCreateInfo *restrict info)
{
	register graph_image_s *restrict r;
	VkDevice device;
	VkResult ret;
	r = (graph_image_s *) refer_alloz(sizeof(graph_image_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_image_free_func);
		r->ml = (mlog_s *) refer_save(heap->ml);
		r->dev = (graph_dev_s *) refer_save(heap->dev);
		r->ga = (graph_allocator_s *) refer_save(heap->ga);
		r->heap = (graph_memory_heap_s *) refer_save(heap);
		ret = vkCreateImage(device = r->dev->dev, info, &r->ga->alloc, &r->image);
		if (!ret)
		{
			r->type = info->imageType;
			r->format = info->format;
			r->extent = info->extent;
			vkGetImageMemoryRequirements(device, r->image, &r->require);
			r->memory = graph_memory_alloc(heap, &r->require, graph_memory_property_device_local);
			if (r->memory)
			{
				ret = vkBindImageMemory(device, r->image, r->memory, 0);
				if (!ret) return r;
				mlog_printf(r->ml, "[graph_image_alloc] vkBindImageMemory = %d\n", ret);
			}
		}
		else mlog_printf(r->ml, "[graph_image_alloc] vkCreateImage = %d\n", ret);
		refer_free(r);
	}
	return NULL;
}

graph_image_s* graph_image_alloc_1d(struct graph_memory_heap_s *restrict heap, graph_format_t format, uint32_t length, const graph_image_param_s *restrict param)
{
	VkImageCreateInfo info;
	if (format < graph_format$number && length)
	{
		graph_image_info_init_data(&info, param);
		info.imageType = VK_IMAGE_TYPE_1D;
		info.format = graph_format2vk(format);
		info.extent.width = length;
		return graph_image_alloc(heap, &info);
	}
	return NULL;
}

graph_image_s* graph_image_alloc_2d(struct graph_memory_heap_s *restrict heap, graph_format_t format, uint32_t width, uint32_t height, const graph_image_param_s *restrict param)
{
	VkImageCreateInfo info;
	if (format < graph_format$number && width && height)
	{
		graph_image_info_init_data(&info, param);
		info.imageType = VK_IMAGE_TYPE_2D;
		info.format = graph_format2vk(format);
		info.extent.width = width;
		info.extent.height = height;
		return graph_image_alloc(heap, &info);
	}
	return NULL;
}

graph_image_s* graph_image_alloc_3d(struct graph_memory_heap_s *restrict heap, graph_format_t format, uint32_t width, uint32_t height, uint32_t depth, const graph_image_param_s *restrict param)
{
	VkImageCreateInfo info;
	if (format < graph_format$number && width && height && depth)
	{
		graph_image_info_init_data(&info, param);
		info.imageType = VK_IMAGE_TYPE_3D;
		info.format = graph_format2vk(format);
		info.extent.width = width;
		info.extent.height = height;
		info.extent.depth = depth;
		return graph_image_alloc(heap, &info);
	}
	return NULL;
}

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

graph_image_view_s* graph_image_view_alloc(register const graph_image_view_param_s *restrict param, register graph_image_s *restrict image)
{
	register graph_image_view_s *restrict r;
	VkImageViewCreateInfo info;
	VkResult ret;
	r = (graph_image_view_s *) refer_alloz(sizeof(graph_image_view_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_image_view_free_func);
		r->ml = (mlog_s *) refer_save(image->ml);
		r->depend = refer_save(image);
		r->dev = (graph_dev_s *) refer_save(image->dev);
		r->image = image->image;
		r->ga = (graph_allocator_s *) refer_save(image->ga);
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.pNext = NULL;
		info.flags = param->flags;
		info.image = r->image;
		info.viewType = param->type;
		info.format = param->format?param->format:image->format;
		info.components = param->components;
		info.subresourceRange = param->range;
		ret = vkCreateImageView(image->dev->dev, &info, &r->ga->alloc, &r->view);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_image_view_alloc] vkCreateImageView = %d\n", ret);
		refer_free(r);
	}
	return NULL;
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

graph_sampler_param_s* graph_sampler_param_alloc(void)
{
	register graph_sampler_param_s *restrict r;
	r = (graph_sampler_param_s *) refer_alloz(sizeof(graph_sampler_param_s));
	if (r) r->info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	return r;
}

void graph_sampler_param_set_flags(register graph_sampler_param_s *restrict param, graph_sampler_flags_t flags)
{
	param->info.flags = (VkSamplerCreateFlags) flags;
}

void graph_sampler_param_set_filter(register graph_sampler_param_s *restrict param, graph_filter_t mag, graph_filter_t min)
{
	param->info.magFilter = graph_filter2vk(mag);
	param->info.minFilter = graph_filter2vk(min);
}

void graph_sampler_param_set_mipmap(register graph_sampler_param_s *restrict param, graph_sampler_mipmap_mode_t mode, float mip_lod_bias, float min_lod, float max_lod)
{
	param->info.mipmapMode = graph_sampler_mipmap_mode2vk(mode);
	param->info.mipLodBias = mip_lod_bias;
	param->info.minLod = min_lod;
	param->info.maxLod = max_lod;
}

void graph_sampler_param_set_address(register graph_sampler_param_s *restrict param, graph_sampler_address_mode_t u, graph_sampler_address_mode_t v, graph_sampler_address_mode_t w)
{
	param->info.addressModeU = graph_sampler_address_mode2vk(u);
	param->info.addressModeV = graph_sampler_address_mode2vk(v);
	param->info.addressModeW = graph_sampler_address_mode2vk(w);
}

void graph_sampler_param_set_anisotropy(register graph_sampler_param_s *restrict param, graph_bool_t enable, float max)
{
	param->info.anisotropyEnable = !!enable;
	param->info.maxAnisotropy = max;
}

void graph_sampler_param_set_compare(register graph_sampler_param_s *restrict param, graph_bool_t enable, graph_compare_op_t op)
{
	param->info.compareEnable = !!enable;
	param->info.compareOp = graph_compare_op2vk(op);
}

void graph_sampler_param_set_border(register graph_sampler_param_s *restrict param, graph_border_color_t color)
{
	if ((uint32_t) color < graph_border_color$number)
		param->info.borderColor = graph_border_color2vk(color);
}

void graph_sampler_param_set_coordinates(register graph_sampler_param_s *restrict param, graph_bool_t unnormalized)
{
	param->info.unnormalizedCoordinates = !!unnormalized;
}

static void graph_sampler_free_func(register graph_sampler_s *restrict r)
{
	register void *v;
	if ((v = r->sampler)) vkDestroySampler(r->dev->dev, (VkSampler) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_sampler_s* graph_sampler_alloc(register const graph_sampler_param_s *restrict param, register struct graph_dev_s *restrict dev)
{
	register graph_sampler_s *restrict r;
	VkResult ret;
	r = (graph_sampler_s *) refer_alloz(sizeof(graph_sampler_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_sampler_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		ret = vkCreateSampler(dev->dev, &param->info, &r->ga->alloc, &r->sampler);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_sampler_alloc] vkCreateSampler = %d\n", ret);
		refer_free(r);
	}
	return NULL;
}
