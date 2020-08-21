#include "surface_pri.h"
#include "device_pri.h"
#include "type_pri.h"
#include <stdlib.h>

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
	register refer_t r;
	if (surface->surface)
		vkDestroySurfaceKHR(surface->g->instance, surface->surface, &surface->ga->alloc);
	if ((r = surface->ga)) refer_free(r);
	if ((r = surface->g)) refer_free(r);
	if ((r = surface->ml)) refer_free(r);
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

