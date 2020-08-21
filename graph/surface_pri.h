#ifndef _graph_surface_pri_h_
#define _graph_surface_pri_h_

#include "surface.h"
#include <vulkan/vulkan.h>
#include "graph_pri.h"

struct graph_surface_s {
	mlog_s *ml;
	graph_s *g;
	VkSurfaceKHR surface;
	graph_allocator_s *ga;
};

struct graph_surface_attr_s {
	mlog_s *ml;
	uint32_t format_number;
	uint32_t mode_number;
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR *formats;
	VkPresentModeKHR *modes;
};

typedef VkResult (*graph_surface_vk_create_f)(VkInstance instance, const void *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface);

const graph_s* graph_surface_init_check(const graph_s *restrict g);
graph_surface_s* graph_surface_init(graph_surface_s *restrict surface, const graph_s *restrict g, graph_surface_vk_create_f func, const void *restrict info);
void graph_surface_uini(graph_surface_s *restrict surface);

#endif
