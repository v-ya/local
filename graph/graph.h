#ifndef _graph_h_
#define _graph_h_

#include <refer.h>
#include <mlog.h>
#include "type.h"

typedef struct graph_s graph_s;

struct graph_allocator_s;

graph_s* graph_alloc(mlog_s *ml, struct graph_allocator_s *ga);
graph_s* graph_init_instance(graph_s *restrict g, const char *restrict app_name, const char *restrict engine_name, const graph_layer_t *restrict layer, const graph_extension_t *restrict extension);
graph_s* graph_instance_enable_debug_message(graph_s *restrict g, graph_debug_message_level_t level, graph_debug_message_type_t type);

#endif
