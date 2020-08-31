#ifndef _graph_surface_pri_h_
#define _graph_surface_pri_h_

#include "surface.h"
#include <vulkan/vulkan.h>
#include "graph_pri.h"
#include "device_pri.h"

typedef const graph_surface_s* (*graph_surface_geometry_f)(const graph_surface_s *restrict surface, graph_surface_geometry_t *restrict geometry);

struct graph_surface_s {
	mlog_s *ml;
	graph_s *g;
	VkSurfaceKHR surface;
	graph_allocator_s *ga;
	graph_surface_geometry_f geometry;
};

struct graph_surface_attr_s {
	mlog_s *ml;
	uint32_t format_number;
	uint32_t mode_number;
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR *formats;
	VkPresentModeKHR *modes;
};

struct graph_swapchain_param_s {
	graph_surface_s *surface;
	graph_surface_attr_s *attr;
	graph_dev_s *dev;
	VkSwapchainCreateInfoKHR info;
	uint32_t qf_number;
	uint32_t queue_family_indices[];
};

struct graph_swapchain_s {
	mlog_s *ml;
	graph_dev_s *dev;
	graph_surface_s *surface;
	VkSwapchainKHR swapchain;
	graph_allocator_s *ga;
	VkSwapchainCreateInfoKHR *info;
	VkImage *image_array;
	uint32_t image_number;
};

typedef VkResult (*graph_surface_vk_create_f)(VkInstance instance, const void *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface);

const graph_s* graph_surface_init_check(const graph_s *restrict g);
graph_surface_s* graph_surface_init(graph_surface_s *restrict surface, const graph_s *restrict g, graph_surface_vk_create_f func, const void *restrict info);
void graph_surface_uini(graph_surface_s *restrict surface);

#endif
