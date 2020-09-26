#ifndef _graph_layout_h_
#define _graph_layout_h_

#include <refer.h>
#include "type.h"

typedef struct graph_descriptor_set_layout_param_s graph_descriptor_set_layout_param_s;
typedef struct graph_descriptor_set_layout_s graph_descriptor_set_layout_s;
typedef struct graph_pipe_layout_param_s graph_pipe_layout_param_s;
typedef struct graph_pipe_layout_s graph_pipe_layout_s;
typedef struct graph_descriptor_pool_s graph_descriptor_pool_s;
typedef struct graph_descriptor_sets_s graph_descriptor_sets_s;
typedef struct graph_descriptor_sets_info_s graph_descriptor_sets_info_s;

struct graph_dev_s;
struct graph_sampler_s;
struct graph_image_view_s;
struct graph_buffer_s;

graph_descriptor_set_layout_param_s* graph_descriptor_set_layout_param_alloc(uint32_t binding_number, uint32_t sampler_number, graph_desc_set_layout_flags_t flags);
graph_descriptor_set_layout_param_s* graph_descriptor_set_layout_param_set_binding(graph_descriptor_set_layout_param_s *restrict param, uint32_t index, uint32_t binding, graph_desc_type_t type, uint32_t number, graph_shader_stage_flags_t flags);

graph_descriptor_set_layout_s* graph_descriptor_set_layout_alloc(graph_descriptor_set_layout_param_s *restrict param, struct graph_dev_s *restrict dev);

graph_pipe_layout_param_s* graph_pipe_layout_param_alloc(uint32_t set_layout_number, uint32_t range_number);
graph_pipe_layout_param_s* graph_pipe_layout_param_append_set_layout(graph_pipe_layout_param_s *restrict param, const graph_descriptor_set_layout_s *restrict set_layout);
graph_pipe_layout_param_s* graph_pipe_layout_param_append_range(graph_pipe_layout_param_s *restrict param, graph_shader_stage_flags_t flags, uint32_t offset, uint32_t size);

graph_pipe_layout_s* graph_pipe_layout_alloc(struct graph_dev_s *restrict dev, const graph_pipe_layout_param_s *restrict param);

graph_descriptor_pool_s* graph_descriptor_pool_alloc(struct graph_dev_s *restrict dev, graph_desc_pool_flags_t flags, uint32_t max_sets, uint32_t pool_size_number, graph_desc_type_t pool_type[], uint32_t pool_desc_number[]);
graph_descriptor_sets_s* graph_descriptor_sets_alloc(graph_descriptor_pool_s *restrict pool, uint32_t number, graph_descriptor_set_layout_s *set_layout[]);

graph_descriptor_sets_info_s* graph_descriptor_sets_info_alloc(graph_descriptor_sets_s *restrict sets, uint32_t n_write, uint32_t n_copy, uint32_t n_image_info, uint32_t n_buffer_info, uint32_t n_buffer_view);
uint32_t graph_descriptor_sets_info_append_write(graph_descriptor_sets_info_s *restrict info, uint32_t set_index, uint32_t binding, graph_desc_type_t type, uint32_t offset, uint32_t count);
void graph_descriptor_sets_info_set_write_image_info(register graph_descriptor_sets_info_s *restrict info, uint32_t write_index, uint32_t image_info_index, const struct graph_sampler_s *restrict sampler, const struct graph_image_view_s *restrict image_view, graph_image_layout_t layout);
void graph_descriptor_sets_info_set_write_buffer_info(graph_descriptor_sets_info_s *restrict info, uint32_t write_index, uint32_t buffer_info_index, const struct graph_buffer_s *restrict buffer, uint64_t offset, uint64_t size);
void graph_descriptor_sets_info_update(graph_descriptor_sets_info_s *restrict info);

#endif
