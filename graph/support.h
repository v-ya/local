#ifndef _graph_support_h_
#define _graph_support_h_

#include <refer.h>
#include <mlog.h>

#include "type.h"

typedef struct graph_layer_support_s graph_layer_support_s;
typedef struct graph_extension_support_s graph_extension_support_s;

struct graph_device_t;

graph_layer_support_s* graph_layer_support_get_instance(void);
graph_layer_support_s* graph_layer_support_get_dev(const struct graph_device_t *restrict device);
graph_extension_support_s* graph_extension_support_get_instance(graph_layer_t layer);
graph_extension_support_s* graph_extension_support_get_dev(const struct graph_device_t *restrict device, graph_layer_t layer);

const graph_layer_support_s* graph_layer_support_test(const graph_layer_support_s *restrict layer_support, graph_layer_t layer, const char *restrict *restrict name, const char *restrict *restrict description, uint32_t *restrict spec_version, uint32_t *restrict impl_version);
const graph_layer_t* graph_layer_support_all(const graph_layer_support_s *restrict layer_support, uint32_t *restrict number);
void graph_layer_support_dump(const graph_layer_support_s *restrict layer_support, mlog_s *restrict ml);

const graph_extension_support_s* graph_extension_support_test(const graph_extension_support_s *restrict extension_support, graph_extension_t extension, const char *restrict *restrict name, uint32_t *restrict spec_version);
const graph_extension_t* graph_extension_support_all(const graph_extension_support_s *restrict extension_support, uint32_t *restrict number);
void graph_extension_support_dump(const graph_extension_support_s *restrict extension_support, mlog_s *restrict ml);

#endif
