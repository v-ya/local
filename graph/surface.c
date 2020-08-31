#include "surface_pri.h"
#include "type_pri.h"
#include "command_pri.h"
#include <stdlib.h>
#include <memory.h>

const graph_s* graph_surface_init_check(const graph_s *restrict g)
{
	if (g->instance) return g;
	mlog_printf(g->ml, "[graph_surface_init_check] instance = null\n");
	return NULL;
}

graph_surface_s* graph_surface_init(register graph_surface_s *restrict surface, const graph_s *restrict g, graph_surface_vk_create_f func, const void *restrict info)
{
	VkResult r;
	r = func(g->instance, info, &g->ga->alloc, &surface->surface);
	if (r) goto label_fail;
	surface->ml = (mlog_s *) refer_save(g->ml);
	surface->g = (graph_s *) refer_save(g);
	surface->ga = (graph_allocator_s *) refer_save(g->ga);
	return surface;
	label_fail:
	mlog_printf(g->ml, "[graph_surface_init] vkCreate*SurfaceKHR = %d\n", r);
	return NULL;
}

void graph_surface_uini(register graph_surface_s *restrict surface)
{
	register void *v;
	if ((v = surface->surface))
		vkDestroySurfaceKHR(surface->g->instance, (VkSurfaceKHR) v, &surface->ga->alloc);
	if ((v = surface->ga)) refer_free(v);
	if ((v = surface->g)) refer_free(v);
	if ((v = surface->ml)) refer_free(v);
}

static inline graph_surface_s* graph_surface_not_support(register const graph_surface_s *restrict surface, register const char *restrict func_name)
{
	mlog_printf(surface->ml, "[%s] surface not support\n", func_name);
	return NULL;
}

static void graph_surface_attr_free_func(register graph_surface_attr_s *restrict r)
{
	if (r->formats) free(r->formats);
	if (r->modes) free(r->modes);
	if (r->ml) refer_free(r->ml);
}

graph_surface_attr_s* graph_surface_attr_get(register const graph_surface_s *restrict surface, register const struct graph_device_t *restrict device)
{
	register graph_surface_attr_s *restrict r;
	VkResult ret;
	r = (graph_surface_attr_s *) refer_alloz(sizeof(graph_surface_attr_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_surface_attr_free_func);
		r->ml = (mlog_s *) refer_save(surface->ml);
		ret = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->phydev, surface->surface, &r->capabilities);
		if (ret) goto label_capabilities;
		ret = vkGetPhysicalDeviceSurfaceFormatsKHR(device->phydev, surface->surface, &r->format_number, NULL);
		if (ret) goto label_formats;
		ret = vkGetPhysicalDeviceSurfacePresentModesKHR(device->phydev, surface->surface, &r->mode_number, NULL);
		if (ret) goto label_modes;
		if (r->format_number)
		{
			r->formats = (VkSurfaceFormatKHR *) malloc(sizeof(VkSurfaceFormatKHR) * r->format_number);
			if (!r->formats) goto label_malloc;
			ret = vkGetPhysicalDeviceSurfaceFormatsKHR(device->phydev, surface->surface, &r->format_number, r->formats);
			if (ret) goto label_formats;
		}
		if (r->mode_number)
		{
			r->modes = (VkPresentModeKHR *) malloc(sizeof(VkPresentModeKHR) * r->mode_number);
			if (!r->modes) goto label_malloc;
			ret = vkGetPhysicalDeviceSurfacePresentModesKHR(device->phydev, surface->surface, &r->mode_number, r->modes);
			if (ret) goto label_modes;
		}
		return r;
		label_return:
		refer_free(r);
	}
	return NULL;
	label_capabilities:
	mlog_printf(surface->ml, "[graph_surface_attr_get] vkGetPhysicalDeviceSurfaceCapabilitiesKHR = %d\n", ret);
	goto label_return;
	label_formats:
	mlog_printf(surface->ml, "[graph_surface_attr_get] vkGetPhysicalDeviceSurfaceFormatsKHR = %d\n", ret);
	goto label_return;
	label_modes:
	mlog_printf(surface->ml, "[graph_surface_attr_get] vkGetPhysicalDeviceSurfacePresentModesKHR = %d\n", ret);
	goto label_return;
	label_malloc:
	mlog_printf(surface->ml, "[graph_surface_attr_get] malloc fail\n");
	goto label_return;
}

graph_surface_s* graph_surface_geometry(register const graph_surface_s *restrict surface, register graph_surface_geometry_t *restrict geometry)
{
	if (surface->geometry)
		return (graph_surface_s *) surface->geometry(surface, geometry);
	return graph_surface_not_support(surface, "graph_surface_geometry");
}

static const VkPresentModeKHR* graph_surface_attr_test_VkPresentModeKHR(register const VkPresentModeKHR *restrict p, register uint32_t n, register VkPresentModeKHR mode)
{
	while (n)
	{
		--n;
		if (*p == mode) return p;
		++p;
	}
	return NULL;
}

static void graph_swapchain_param_free_func(register graph_swapchain_param_s *restrict r)
{
	register refer_t v;
	if ((v = r->surface)) refer_free(v);
	if ((v = r->attr)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
}

graph_swapchain_param_s* graph_swapchain_param_alloc(register graph_surface_s *restrict surface, register const graph_surface_attr_s *restrict attr, uint32_t queue_sharing_number)
{
	register graph_swapchain_param_s *restrict r;
	r = NULL;
	if (attr->format_number && attr->mode_number)
	{
		r = (graph_swapchain_param_s *) refer_alloz(sizeof(graph_swapchain_param_s) + sizeof(uint32_t) * queue_sharing_number);
		if (r)
		{
			refer_set_free(r, (refer_free_f) graph_swapchain_param_free_func);
			r->surface = (graph_surface_s *) refer_save(surface);
			r->attr = (graph_surface_attr_s *) refer_save(attr);
			r->qf_number = queue_sharing_number;
			r->info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			r->info.pNext = NULL;
			r->info.flags = 0;
			r->info.surface = surface->surface;
			r->info.minImageCount = attr->capabilities.minImageCount + 1;
			if (r->info.minImageCount > attr->capabilities.maxImageCount)
				r->info.minImageCount = attr->capabilities.maxImageCount;
			r->info.imageFormat = attr->formats->format;
			r->info.imageColorSpace = attr->formats->colorSpace;
			r->info.imageExtent = attr->capabilities.currentExtent;
			r->info.imageArrayLayers = attr->capabilities.maxImageArrayLayers;
			r->info.imageUsage = attr->capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			r->info.imageSharingMode = queue_sharing_number?VK_SHARING_MODE_CONCURRENT:VK_SHARING_MODE_EXCLUSIVE;
			r->info.queueFamilyIndexCount = 0;
			r->info.pQueueFamilyIndices = queue_sharing_number?r->queue_family_indices:NULL;
			r->info.preTransform = attr->capabilities.currentTransform;
			r->info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			if (graph_surface_attr_test_VkPresentModeKHR(attr->modes, attr->mode_number, VK_PRESENT_MODE_MAILBOX_KHR))
				r->info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			else if (graph_surface_attr_test_VkPresentModeKHR(attr->modes, attr->mode_number, VK_PRESENT_MODE_FIFO_KHR))
				r->info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
			else r->info.presentMode = *attr->modes;
			r->info.clipped = VK_TRUE;
			r->info.oldSwapchain = NULL;
		}
	}
	return r;
}

static void graph_swapchain_free_func(register graph_swapchain_s *restrict r)
{
	register void *v;
	// if ((v = r->info->oldSwapchain))
	// 	vkDestroySwapchainKHR(r->dev->dev, (VkSwapchainKHR) v, &r->ga->alloc);
	if ((v = r->swapchain))
		vkDestroySwapchainKHR(r->dev->dev, (VkSwapchainKHR) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->surface)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
	if ((v = r->info)) free(v);
	if ((v = r->image_array)) free(v);
}

graph_swapchain_s* graph_swapchain_alloc(register const graph_swapchain_param_s *restrict param, register struct graph_dev_s *dev)
{
	register graph_swapchain_s *restrict r;
	VkResult ret;
	if ((dev || param->dev) && (!dev || !param->dev || dev == param->dev))
	{
		r = (graph_swapchain_s *) refer_alloz(sizeof(graph_swapchain_s));
		if (r)
		{
			r->info = malloc(sizeof(VkSwapchainCreateInfoKHR) + sizeof(uint32_t) * param->info.queueFamilyIndexCount);
			if (r->info)
			{
				refer_set_free(r, (refer_free_f) graph_swapchain_free_func);
				memcpy(r->info, &param->info, sizeof(VkSwapchainCreateInfoKHR));
				r->info->pQueueFamilyIndices = NULL;
				if (param->info.queueFamilyIndexCount)
					r->info->pQueueFamilyIndices = (uint32_t *) memcpy(r->info + 1,
						param->info.pQueueFamilyIndices,
						sizeof(uint32_t) * param->info.queueFamilyIndexCount);
				if (!dev) dev = param->dev;
				r->ml = (mlog_s *) refer_save(dev->ml);
				r->ga = (graph_allocator_s *) refer_save(dev->ga);
				r->dev = (graph_dev_s *) refer_save(dev);
				r->surface = (graph_surface_s *) refer_save(param->surface);
				ret = vkCreateSwapchainKHR(dev->dev, r->info, &r->ga->alloc, &r->swapchain);
				if (ret) goto label_fail;
				ret = vkGetSwapchainImagesKHR(dev->dev, r->swapchain, &r->image_number, NULL);
				if (ret) goto label_image;
				r->image_array = (VkImage *) malloc(sizeof(VkImage) * r->image_number);
				if (!r->image_array) goto label_malloc;
				ret = vkGetSwapchainImagesKHR(dev->dev, r->swapchain, &r->image_number, r->image_array);
				if (ret) goto label_image;
				return r;
			}
			label_free:
			refer_free(r);
		}
	}
	return NULL;
	label_fail:
	mlog_printf(r->ml, "[graph_swapchain_alloc] vkCreateSwapchainKHR = %d\n", ret);
	goto label_free;
	label_image:
	mlog_printf(r->ml, "[graph_swapchain_alloc] vkGetSwapchainImagesKHR = %d\n", ret);
	goto label_free;
	label_malloc:
	mlog_printf(r->ml, "[graph_swapchain_alloc] malloc fail\n");
	goto label_free;
}

graph_swapchain_s* graph_swapchain_rebulid(register graph_swapchain_s *restrict swapchain)
{
	register VkSwapchainCreateInfoKHR *restrict info;
	VkImage *image_array;
	graph_surface_geometry_t geometry;
	uint32_t n;
	VkResult ret;
	info = swapchain->info;
	if (graph_surface_geometry(swapchain->surface, &geometry) && geometry.width && geometry.height)
	{
		if (geometry.width != info->imageExtent.width || geometry.height != info->imageExtent.height)
		{
			info->imageExtent.width = geometry.width;
			info->imageExtent.height = geometry.height;
			swapchain->info->oldSwapchain = swapchain->swapchain;
			ret = vkCreateSwapchainKHR(swapchain->dev->dev, swapchain->info, &swapchain->ga->alloc, &swapchain->swapchain);
			if (!ret)
			{
				image_array = NULL;
				ret = vkGetSwapchainImagesKHR(swapchain->dev->dev, swapchain->swapchain, &n, NULL);
				if (ret) goto label_image;
				image_array = (VkImage *) malloc(sizeof(VkImage) * n);
				if (!image_array) goto label_malloc;
				ret = vkGetSwapchainImagesKHR(swapchain->dev->dev, swapchain->swapchain, &n, image_array);
				if (ret) goto label_image;
				swapchain->image_number = n;
				free(swapchain->image_array);
				swapchain->image_array = image_array;
				vkDestroySwapchainKHR(swapchain->dev->dev, swapchain->info->oldSwapchain, &swapchain->ga->alloc);
				swapchain->info->oldSwapchain = NULL;
			}
			else
			{
				mlog_printf(swapchain->ml, "[graph_swapchain_rebulid] vkCreateSwapchainKHR = %d\n", ret);
				label_swap:
				swapchain->swapchain = swapchain->info->oldSwapchain;
				swapchain->info->oldSwapchain = NULL;
				goto label_fail;
				label_image:
				if (image_array) free(image_array);
				mlog_printf(swapchain->ml, "[graph_swapchain_rebulid] vkGetSwapchainImagesKHR = %d\n", ret);
				goto label_swap;
				label_malloc:
				mlog_printf(swapchain->ml, "[graph_swapchain_rebulid] malloc fail\n");
				goto label_swap;
			}
		}
		return swapchain;
	}
	label_fail:
	return NULL;
}

uint32_t graph_swapchain_image_number(register const graph_swapchain_s *restrict swapchain)
{
	return swapchain->image_number;
}

graph_format_t graph_swapchain_format(register const graph_swapchain_s *restrict swapchain)
{
	return graph_format4vk(swapchain->info->imageFormat);
}

void graph_swapchain_info(register const graph_swapchain_s *restrict swapchain, uint32_t *restrict image_number, graph_format_t *restrict format, uint32_t *restrict width, uint32_t *restrict height)
{
	register const VkSwapchainCreateInfoKHR *restrict info;
	info = swapchain->info;
	if (image_number) *image_number = swapchain->image_number;
	if (format) *format = graph_format4vk(info->imageFormat);
	if (width) *width = info->imageExtent.width;
	if (height) *height = info->imageExtent.height;
}

uint32_t graph_swapchain_acquire(register graph_swapchain_s *restrict swapchain, uint64_t timeout, struct graph_semaphore_s *restrict semaphore, struct graph_fence_s *restrict fence)
{
	uint32_t r;
	VkResult ret;
	ret = vkAcquireNextImageKHR(
		swapchain->dev->dev,
		swapchain->swapchain,
		timeout,
		semaphore?semaphore->semaphore:NULL,
		fence?fence->fence:NULL,
		&r
	);
	if (!ret) return r;
	mlog_printf(swapchain->ml, "[graph_swapchain_acquire] vkAcquireNextImageKHR = %d\n", ret);
	return ~0;
}

static void graph_surface_attr_dump_capabilities(register mlog_s *restrict ml, register const VkSurfaceCapabilitiesKHR *restrict r)
{
	char buffer[128];
	mlog_printf(ml, "\t" "min image count           = %u\n", r->minImageCount);
	mlog_printf(ml, "\t" "max image count           = %u\n", r->maxImageCount);
	mlog_printf(ml, "\t" "current extent            = [w:%u, h:%u]\n", r->currentExtent.width, r->currentExtent.height);
	mlog_printf(ml, "\t" "min image extent          = [w:%u, h:%u]\n", r->minImageExtent.width, r->minImageExtent.height);
	mlog_printf(ml, "\t" "max image extent          = [w:%u, h:%u]\n", r->maxImageExtent.width, r->maxImageExtent.height);
	mlog_printf(ml, "\t" "max image array layers    = %u\n", r->maxImageArrayLayers);
	mlog_printf(ml, "\t" "supported transforms      = 0x%04x (%s)\n",
		r->supportedTransforms, graph_surface_transform$list(buffer, (graph_surface_transform_t) r->supportedTransforms));
	mlog_printf(ml, "\t" "current transform         = 0x%04x (%s)\n",
		r->currentTransform, graph_surface_transform$list(buffer, (graph_surface_transform_t) r->currentTransform));
	mlog_printf(ml, "\t" "supported composite alpha = 0x%02x (%s)\n",
		r->supportedCompositeAlpha, graph_composite_alpha$list(buffer, (graph_composite_alpha_t) r->supportedCompositeAlpha));
	mlog_printf(ml, "\t" "supported usage flags     = 0x%02x (%s)\n",
		r->supportedUsageFlags, graph_image_usage$list(buffer, (graph_image_usage_t) r->supportedUsageFlags));
}

static void graph_surface_attr_dump_formats(register mlog_s *restrict ml, register const VkSurfaceFormatKHR *restrict r, register uint32_t n)
{
	graph_format_t f;
	do {
		f = graph_format4vk(r->format);
		mlog_printf(ml, "\t" "format: %d (%s), color-space: %d (%s)\n",
			f, graph_format$string(f),
			r->colorSpace,
			graph_color_space$string((graph_color_space_t) r->colorSpace));
		++r;
	} while (--n);
}

static void graph_surface_attr_dump_modes(register mlog_s *restrict ml, register const VkPresentModeKHR *restrict r, register uint32_t n)
{
	do {
		mlog_printf(ml, "\t" "[%d] (%s)\n",
			*r, graph_present_mode$string((graph_present_mode_t) *r));
		++r;
	} while (--n);
}

void graph_surface_attr_dump(register const graph_surface_attr_s *restrict attr)
{
	mlog_printf(attr->ml, "surface capabilities:\n");
	graph_surface_attr_dump_capabilities(attr->ml, &attr->capabilities);
	mlog_printf(attr->ml, "surface formats:\n");
	if (attr->format_number) graph_surface_attr_dump_formats(attr->ml, attr->formats, attr->format_number);
	mlog_printf(attr->ml, "present modes:\n");
	if (attr->mode_number) graph_surface_attr_dump_modes(attr->ml, attr->modes, attr->mode_number);
}

