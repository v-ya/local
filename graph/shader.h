#ifndef _graph_shader_h_
#define _graph_shader_h_

#include <refer.h>
#include "type.h"

typedef struct graph_shader_s graph_shader_s;
typedef struct graph_shader_spec_s graph_shader_spec_s;
typedef struct graph_vertex_input_description_s graph_vertex_input_description_s;
typedef struct graph_viewports_scissors_s graph_viewports_scissors_s;
typedef struct graph_pipe_color_blend_s graph_pipe_color_blend_s;
typedef struct graph_render_pass_param_s graph_render_pass_param_s;
typedef struct graph_render_pass_s graph_render_pass_s;
typedef struct graph_pipe_cache_s graph_pipe_cache_s;
typedef struct graph_pipe_s graph_pipe_s;
typedef struct graph_gpipe_param_s graph_gpipe_param_s;

struct graph_dev_s;
struct graph_pipe_layout_s;

// size == 0 (code => path), size > 0 (code => data)
graph_shader_s* graph_shader_alloc(struct graph_dev_s *restrict dev, const void *restrict code, size_t size);

graph_vertex_input_description_s* graph_vertex_input_description_alloc(uint32_t bind_number, uint32_t attr_number);
graph_vertex_input_description_s* graph_vertex_input_description_set_bind(graph_vertex_input_description_s *restrict desc, uint32_t index, uint32_t binding, uint32_t stride, graph_vertex_input_rate_t irate);
graph_vertex_input_description_s* graph_vertex_input_description_set_attr(graph_vertex_input_description_s *restrict desc, uint32_t index, uint32_t location, uint32_t binding, uint32_t offset, graph_format_t format);

graph_viewports_scissors_s* graph_viewports_scissors_alloc(uint32_t viewport_number, uint32_t scissor_number);
graph_viewports_scissors_s* graph_viewports_scissors_append_viewport(graph_viewports_scissors_s *restrict r, float x, float y, float w, float h, float dmin, float dmax);
graph_viewports_scissors_s* graph_viewports_scissors_append_scissor(graph_viewports_scissors_s *restrict r, int32_t x, int32_t y, uint32_t w, uint32_t h);
graph_viewports_scissors_s* graph_viewports_scissors_update_viewport(graph_viewports_scissors_s *restrict r, uint32_t index, float x, float y, float w, float h, float dmin, float dmax);
graph_viewports_scissors_s* graph_viewports_scissors_update_scissor(graph_viewports_scissors_s *restrict r, uint32_t index, int32_t x, int32_t y, uint32_t w, uint32_t h);

graph_pipe_color_blend_s* graph_pipe_color_blend_alloc(uint32_t attachment_number);
void graph_pipe_color_blend_set_logic(graph_pipe_color_blend_s *restrict r, graph_bool_t logic_op, graph_logic_op_t op);
void graph_pipe_color_blend_set_constants(graph_pipe_color_blend_s *restrict r, const float constants[4]);
graph_pipe_color_blend_s* graph_pipe_color_blend_append_attachment(graph_pipe_color_blend_s *restrict r, graph_bool_t enable, graph_color_component_mask_t mask, graph_blend_factor_t s_color, graph_blend_factor_t d_color, graph_blend_op_t op_color, graph_blend_factor_t s_alpha, graph_blend_factor_t d_alpha, graph_blend_op_t op_alpha);

graph_render_pass_param_s* graph_render_pass_param_alloc(uint32_t attachment_number, uint32_t subpass_number, uint32_t dependency_number, uint32_t aref_number);
graph_render_pass_param_s* graph_render_pass_param_set_attachment(graph_render_pass_param_s *restrict r, uint32_t index, graph_format_t format, graph_sample_count_t sample, graph_attachment_load_op_t load, graph_attachment_store_op_t store, graph_attachment_load_op_t stencil_load, graph_attachment_store_op_t stencil_store, graph_image_layout_t initial, graph_image_layout_t final);
graph_render_pass_param_s* graph_render_pass_param_set_subpass(graph_render_pass_param_s *restrict r, uint32_t index, graph_pipeline_bind_point_t type);
graph_render_pass_param_s* graph_render_pass_param_set_subpass_color(graph_render_pass_param_s *restrict r, uint32_t index, uint32_t n, uint32_t at_index[], graph_image_layout_t layout[]);
graph_render_pass_s* graph_render_pass_alloc(graph_render_pass_param_s *restrict param, struct graph_dev_s *restrict dev);

graph_pipe_cache_s* graph_pipe_cache_alloc(struct graph_dev_s *restrict dev);

graph_pipe_s* graph_pipe_alloc_graphics(const graph_gpipe_param_s *restrict param, const graph_pipe_cache_s *restrict cache);
graph_gpipe_param_s* graph_gpipe_param_alloc(struct graph_dev_s *restrict dev, uint32_t shader_number);
graph_gpipe_param_s* graph_gpipe_param_append_shader(graph_gpipe_param_s *restrict r, const graph_shader_s *restrict shader, graph_shader_type_t type, const char *restrict entry, const graph_shader_spec_s *restrict spec);
void graph_gpipe_param_set_vertex(graph_gpipe_param_s *restrict r, const graph_vertex_input_description_s *restrict vertex);
graph_gpipe_param_s* graph_gpipe_param_set_assembly(graph_gpipe_param_s *restrict r, graph_primitive_topology_t pt, graph_bool_t primitive_restart);
graph_gpipe_param_s* graph_gpipe_param_set_viewport(graph_gpipe_param_s *restrict r, const graph_viewports_scissors_s *restrict gvs);
graph_gpipe_param_s* graph_gpipe_param_set_color_blend(graph_gpipe_param_s *restrict r, const graph_pipe_color_blend_s *restrict blend);
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
graph_gpipe_param_s* graph_gpipe_param_set_dynamic(graph_gpipe_param_s *restrict r, uint32_t n, const graph_dynamic_t dynamic[]);
void graph_gpipe_param_set_layout(graph_gpipe_param_s *restrict r, const struct graph_pipe_layout_s *restrict layout);
void graph_gpipe_param_set_render(graph_gpipe_param_s *restrict r, const graph_render_pass_s *restrict render, uint32_t subpass_index);
graph_gpipe_param_s* graph_gpipe_param_ok(graph_gpipe_param_s *restrict r);

#endif
