#ifndef _iyii_swapchain_h_
#define _iyii_swapchain_h_

#include "iyii_graph.h"

typedef struct iyii_swapchain_s {
	graph_swapchain_s *swapchain;
	graph_image_view_param_s *image_view_param;
	graph_image_view_s **image_view;
	graph_fence_s **fence;
	graph_semaphore_s **semaphore;
	uint32_t image_number;
	graph_format_t format;
	uint32_t width;
	uint32_t height;
	uint32_t index;
	uint32_t offset;
} iyii_swapchain_s;

iyii_swapchain_s* iyii_swapchain_alloc(graph_dev_s *restrict dev, const graph_device_t *restrict device, graph_surface_s *restrict surface);
iyii_swapchain_s* iyii_swapchain_acquire(iyii_swapchain_s *restrict swapchain, uint32_t *restrict index, uint32_t *restrict i);
iyii_swapchain_s* iyii_swapchain_render(iyii_swapchain_s *restrict swapchain, graph_queue_t *restrict graphics, graph_command_pool_s *restrict cpool_draw, uint32_t index, uint32_t i);
iyii_swapchain_s* iyii_swapchain_rebuild(iyii_swapchain_s *restrict swapchain);

#endif
