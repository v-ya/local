#ifndef _graph_layout_h_
#define _graph_layout_h_

#include <refer.h>
#include "type.h"

typedef struct graph_descriptor_set_layout_param_s graph_descriptor_set_layout_param_s;
typedef struct graph_descriptor_set_layout_s graph_descriptor_set_layout_s;
typedef struct graph_pipe_layout_param_s graph_pipe_layout_param_s;
typedef struct graph_pipe_layout_s graph_pipe_layout_s;

struct graph_dev_s;
struct graph_sampler_s;

graph_descriptor_set_layout_param_s* graph_descriptor_set_layout_param_alloc(uint32_t binding_number, uint32_t sampler_number, graph_desc_set_layout_flags_t flags);
graph_descriptor_set_layout_param_s* graph_descriptor_set_layout_param_set_binding(graph_descriptor_set_layout_param_s *restrict param, uint32_t index, uint32_t binding, graph_desc_type_t type, uint32_t number, graph_shader_stage_flags_t flags);

graph_descriptor_set_layout_s* graph_descriptor_set_layout_alloc(graph_descriptor_set_layout_param_s *restrict param, struct graph_dev_s *restrict dev);

graph_pipe_layout_param_s* graph_pipe_layout_param_alloc(uint32_t set_layout_number, uint32_t range_number);
graph_pipe_layout_param_s* graph_pipe_layout_param_append_set_layout(graph_pipe_layout_param_s *restrict param, const graph_descriptor_set_layout_s *restrict set_layout);
graph_pipe_layout_param_s* graph_pipe_layout_param_append_range(graph_pipe_layout_param_s *restrict param, graph_shader_stage_flags_t flags, uint32_t offset, uint32_t size);

graph_pipe_layout_s* graph_pipe_layout_alloc(struct graph_dev_s *restrict dev, const graph_pipe_layout_param_s *restrict param);

#endif
