#ifndef _graph_image_h_
#define _graph_image_h_

#include <refer.h>
#include "type.h"

typedef struct graph_image_view_param_s graph_image_view_param_s;
typedef struct graph_image_view_s graph_image_view_s;

struct graph_swapchain_s;

graph_image_view_param_s* graph_image_view_param_alloc(graph_image_view_type_t type);
graph_image_view_s* graph_image_view_alloc_by_swapchain(const graph_image_view_param_s *restrict param, struct graph_swapchain_s *restrict swapchain, uint32_t index);

#endif
