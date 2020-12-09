#ifndef _graph_surface_pri_h_
#define _graph_surface_pri_h_

#include "surface.h"
#include <vulkan/vulkan.h>
#include "graph_pri.h"
#include "device_pri.h"

typedef const graph_surface_s* (*graph_surface_do_event_f)(graph_surface_s *restrict surface);
typedef const graph_surface_s* (*graph_surface_set_event_f)(graph_surface_s *restrict surface, const graph_surface_event_t *restrict events);
typedef const graph_surface_s* (*graph_surface_get_geometry_f)(const graph_surface_s *restrict surface, graph_surface_geometry_t *restrict geometry);

typedef struct graph_surface_control_t {
	graph_surface_do_event_f do_event;
	graph_surface_set_event_f set_event;
	graph_surface_get_geometry_f get_geometry;
} graph_surface_control_t;

typedef struct graph_surface_report_t {
	refer_t data;
	graph_surface_do_event_close_f do_close;
	graph_surface_do_event_expose_f do_expose;
	graph_surface_do_event_key_f do_key;
	graph_surface_do_event_button_f do_button;
	graph_surface_do_event_pointer_f do_pointer;
	graph_surface_do_event_area_f do_area;
	graph_surface_do_event_focus_f do_focus;
	graph_surface_do_event_resize_f do_resize;
} graph_surface_report_t;

struct graph_surface_s {
	mlog_s *ml;
	graph_s *g;
	VkSurfaceKHR surface;
	graph_allocator_s *ga;
	graph_surface_control_t control;
	graph_surface_report_t report;
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
