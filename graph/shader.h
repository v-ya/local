#ifndef _graph_shader_h_
#define _graph_shader_h_

#include <refer.h>
#include "type.h"

typedef struct graph_shader_s graph_shader_s;
typedef struct graph_shader_spec_s graph_shader_spec_s;
typedef struct graph_viewports_scissors_s graph_viewports_scissors_s;
typedef struct graph_pipe_layout_param_s graph_pipe_layout_param_s;
typedef struct graph_pipe_layout_s graph_pipe_layout_s;
typedef struct graph_pipe_s graph_pipe_s;
typedef struct graph_gpipe_param_s graph_gpipe_param_s;

struct graph_dev_s;

// size == 0 (code => path), size > 0 (code => data)
graph_shader_s* graph_shader_alloc(struct graph_dev_s *restrict dev, const void *restrict code, size_t size);

graph_viewports_scissors_s* graph_viewports_scissors_alloc(uint32_t viewport_number, uint32_t scissor_number);
graph_viewports_scissors_s* graph_viewports_scissors_append_viewport(graph_viewports_scissors_s *restrict r, float x, float y, float w, float h, float dmin, float dmax);
graph_viewports_scissors_s* graph_viewports_scissors_append_scissor(graph_viewports_scissors_s *restrict r, int32_t x, int32_t y, uint32_t w, uint32_t h);

graph_pipe_layout_s* graph_pipe_layout_alloc(struct graph_dev_s *restrict dev, register const graph_pipe_layout_param_s *restrict param);

graph_gpipe_param_s* graph_gpipe_param_alloc(struct graph_dev_s *restrict dev, uint32_t shader_number);
graph_gpipe_param_s* graph_gpipe_param_append_shader(graph_gpipe_param_s *restrict r, const graph_shader_s *restrict shader, graph_shader_type_t type, const char *restrict entry, const graph_shader_spec_s *restrict spec);
graph_gpipe_param_s* graph_gpipe_param_set_assembly(graph_gpipe_param_s *restrict r, graph_primitive_topology_t pt, graph_bool_t primitive_restart);
graph_gpipe_param_s* graph_gpipe_param_set_viewport(graph_gpipe_param_s *restrict r, const graph_viewports_scissors_s *restrict gvs);
void graph_gpipe_param_set_rasterization_depth_clamp(graph_gpipe_param_s *restrict r, graph_bool_t depth_clamp);
void graph_gpipe_param_set_rasterization_discard(graph_gpipe_param_s *restrict r, graph_bool_t discard);
void graph_gpipe_param_set_rasterization_polygon(graph_gpipe_param_s *restrict r, graph_polygon_mode_t polygon);
void graph_gpipe_param_set_rasterization_cull(graph_gpipe_param_s *restrict r, graph_cull_mode_flags_t cull);
void graph_gpipe_param_set_rasterization_front_face(graph_gpipe_param_s *restrict r, graph_front_face_t front_face);
void graph_gpipe_param_set_rasterization_depth_bias(graph_gpipe_param_s *restrict r, float constant_factor, float clamp, float slope_factor);
void graph_gpipe_param_set_rasterization_line_width(graph_gpipe_param_s *restrict r, float line_width);
void graph_gpipe_param_set_multisample_sample_shading(graph_gpipe_param_s *restrict r, graph_bool_t sample_shading);
void graph_gpipe_param_set_multisample_min_sample_shading(graph_gpipe_param_s *restrict r, float min_sample_shading);
void graph_gpipe_param_set_multisample_alpha2coverage(graph_gpipe_param_s *restrict r, graph_bool_t alpha2coverage);
void graph_gpipe_param_set_multisample_alpha2one(graph_gpipe_param_s *restrict r, graph_bool_t alpha2one);
void graph_gpipe_param_set_layout(graph_gpipe_param_s *restrict r, const graph_pipe_layout_s *restrict layout);

#endif
