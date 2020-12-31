#ifndef _iyii_pipeline_h_
#define _iyii_pipeline_h_

#include "iyii_graph.h"
#include "iyii_swapchain.h"
#include "iyii_render.h"

typedef struct iyii_pipeline_s {
	graph_dev_s *dev;
	graph_pipe_cache_s *cache;
	graph_shader_s *shader_vertex;
	graph_shader_s *shader_fragment;
	graph_vertex_input_description_s *vertex_input_desc;
	graph_viewports_scissors_s *viewports;
	graph_pipe_color_blend_s *color_blend;
	graph_descriptor_set_layout_s *desc_set_layout;
	graph_pipe_layout_s *layout;
	graph_render_pass_s *render_pass;
	graph_gpipe_param_s *gpipe_param;
	graph_pipe_s *gpipe;
	int32_t output_x;
	int32_t output_y;
	uint32_t output_width;
	uint32_t output_height;
	graph_format_t format;
} iyii_pipeline_s;

iyii_pipeline_s* iyii_pipeline_alloc(graph_dev_s *restrict dev, graph_format_t format, int32_t x, int32_t y, uint32_t w, uint32_t h);

#endif
