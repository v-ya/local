#ifndef _graph_image_h_
#define _graph_image_h_

#include <refer.h>
#include "type.h"

typedef struct graph_image_param_s graph_image_param_s;
typedef struct graph_image_s graph_image_s;
typedef struct graph_image_view_param_s graph_image_view_param_s;
typedef struct graph_image_view_s graph_image_view_s;
typedef struct graph_frame_buffer_s graph_frame_buffer_s;
typedef struct graph_sampler_param_s graph_sampler_param_s;
typedef struct graph_sampler_s graph_sampler_s;

struct graph_dev_s;
struct graph_memory_heap_s;
struct graph_swapchain_s;
struct graph_render_pass_s;

graph_image_s* graph_image_alloc_1d(struct graph_memory_heap_s *restrict heap, graph_format_t format, uint32_t length, const graph_image_param_s *restrict param);
graph_image_s* graph_image_alloc_2d(struct graph_memory_heap_s *restrict heap, graph_format_t format, uint32_t width, uint32_t height, const graph_image_param_s *restrict param);
graph_image_s* graph_image_alloc_3d(struct graph_memory_heap_s *restrict heap, graph_format_t format, uint32_t width, uint32_t height, uint32_t depth, const graph_image_param_s *restrict param);

graph_image_view_param_s* graph_image_view_param_alloc(graph_image_view_type_t type);
graph_image_view_s* graph_image_view_alloc(const graph_image_view_param_s *restrict param, graph_image_s *restrict image);
graph_image_view_s* graph_image_view_alloc_by_swapchain(const graph_image_view_param_s *restrict param, struct graph_swapchain_s *restrict swapchain, uint32_t index);
graph_frame_buffer_s* graph_frame_buffer_alloc(struct graph_render_pass_s *restrict render, graph_image_view_s *restrict view, uint32_t width, uint32_t height, uint32_t layers);

graph_sampler_param_s* graph_sampler_param_alloc(void);
void graph_sampler_param_set_flags(graph_sampler_param_s *restrict param, graph_sampler_flags_t flags);
void graph_sampler_param_set_filter(graph_sampler_param_s *restrict param, graph_filter_t mag, graph_filter_t min);
void graph_sampler_param_set_mipmap(graph_sampler_param_s *restrict param, graph_sampler_mipmap_mode_t mode, float mip_lod_bias, float min_lod, float max_lod);
void graph_sampler_param_set_address(graph_sampler_param_s *restrict param, graph_sampler_address_mode_t u, graph_sampler_address_mode_t v, graph_sampler_address_mode_t w);
void graph_sampler_param_set_anisotropy(graph_sampler_param_s *restrict param, graph_bool_t enable, float max);
void graph_sampler_param_set_compare(graph_sampler_param_s *restrict param, graph_bool_t enable, graph_compare_op_t op);
void graph_sampler_param_set_border(graph_sampler_param_s *restrict param, graph_border_color_t color);
void graph_sampler_param_set_coordinates(graph_sampler_param_s *restrict param, graph_bool_t unnormalized);
graph_sampler_s* graph_sampler_alloc(const graph_sampler_param_s *restrict param, struct graph_dev_s *restrict dev);

#endif
