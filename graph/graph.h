#ifndef _graph_h_
#define _graph_h_

#include <refer.h>
#include <mlog.h>

#include "type.h"

typedef struct graph_s graph_s;
typedef struct graph_layer_support_s graph_layer_support_s;
typedef struct graph_extension_support_s graph_extension_support_s;

struct graph_allocator_s;

graph_layer_support_s* graph_layer_support_get(void);
const graph_layer_support_s* graph_layer_support_test(const graph_layer_support_s *restrict layer_support, graph_layer_t layer, const char *restrict *restrict name, const char *restrict *restrict description, uint32_t *restrict spec_version, uint32_t *restrict impl_version);
const graph_layer_t* graph_layer_support_all(const graph_layer_support_s *restrict layer_support, uint32_t *restrict number);
void graph_layer_support_dump(const graph_layer_support_s *restrict layer_support, mlog_s *restrict ml);

graph_extension_support_s* graph_extension_support_get(graph_layer_t layer);
const graph_extension_support_s* graph_extension_support_test(const graph_extension_support_s *restrict extension_support, graph_extension_t extension, const char *restrict *restrict name, uint32_t *restrict spec_version);
const graph_extension_t* graph_extension_support_all(const graph_extension_support_s *restrict extension_support, uint32_t *restrict number);
void graph_extension_support_dump(const graph_extension_support_s *restrict extension_support, mlog_s *restrict ml);

graph_s* graph_alloc(mlog_s *ml, struct graph_allocator_s *ga);
graph_s* graph_init_instance(graph_s *restrict g, const char *restrict app_name, const char *restrict engine_name, const graph_layer_t *restrict layer, const graph_extension_t *restrict extension);
graph_s* graph_instance_enable_debug_message(graph_s *restrict g, graph_debug_message_level_t level, graph_debug_message_type_t type);

#endif
