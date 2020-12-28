#ifndef _iyii_render_h_
#define _iyii_render_h_

#include "iyii_graph.h"
#include "iyii_swapchain.h"

typedef struct iyii_render_s {
	graph_render_pass_s *render_pass;
	iyii_swapchain_s *swapchain;
	uint32_t image_number;
	graph_format_t format;
	uint32_t width;
	uint32_t height;
	graph_frame_buffer_s *frame_buffer[];
} iyii_render_s;

iyii_render_s* iyii_render_alloc(graph_render_pass_s *restrict render_pass, iyii_swapchain_s *restrict swapchain);

#endif
