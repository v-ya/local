#ifndef _iyii_swapchain_h_
#define _iyii_swapchain_h_

#include "iyii_graph.h"

typedef struct iyii_swapchain_s {
	graph_swapchain_s *swapchain;
	graph_image_view_param_s *image_view_param;
	graph_image_view_s **image_view;
	uint32_t image_number;
	graph_format_t format;
	uint32_t width;
	uint32_t height;
} iyii_swapchain_s;

iyii_swapchain_s* iyii_swapchain_alloc(graph_dev_s *restrict dev, const graph_device_t *restrict device, graph_surface_s *restrict surface);

#endif
