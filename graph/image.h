#ifndef _graph_image_h_
#define _graph_image_h_

#include <refer.h>
#include "type.h"

typedef struct graph_image_param_s graph_image_param_s;
typedef struct graph_image_s graph_image_s;
typedef struct graph_image_view_param_s graph_image_view_param_s;
typedef struct graph_image_view_s graph_image_view_s;
typedef struct graph_frame_buffer_s graph_frame_buffer_s;

struct graph_dev_s;
struct graph_memory_heap_s;
struct graph_swapchain_s;
struct graph_render_pass_s;

graph_image_s* graph_image_alloc_1d(struct graph_memory_heap_s *restrict heap, graph_format_t format, uint32_t length, const graph_image_param_s *restrict param);
graph_image_s* graph_image_alloc_2d(struct graph_memory_heap_s *restrict heap, graph_format_t format, uint32_t width, uint32_t height, const graph_image_param_s *restrict param);
graph_image_s* graph_image_alloc_3d(struct graph_memory_heap_s *restrict heap, graph_format_t format, uint32_t width, uint32_t height, uint32_t depth, const graph_image_param_s *restrict param);

graph_image_view_param_s* graph_image_view_param_alloc(graph_image_view_type_t type);
graph_image_view_s* graph_image_view_alloc_by_swapchain(const graph_image_view_param_s *restrict param, struct graph_swapchain_s *restrict swapchain, uint32_t index);
graph_frame_buffer_s* graph_frame_buffer_alloc(struct graph_render_pass_s *restrict render, graph_image_view_s *restrict view, uint32_t width, uint32_t height, uint32_t layers);

#endif
