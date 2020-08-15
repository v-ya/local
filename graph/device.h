#ifndef _graph_device_h_
#define _graph_device_h_

#include <refer.h>
#include <mlog.h>
#include "graph.h"

typedef struct graph_devices_s graph_devices_s;
typedef struct graph_device_t graph_device_t;

graph_devices_s* graph_instance_devices_get(const graph_s *restrict g);
uint32_t graph_devices_number(const graph_devices_s *restrict gds);
const graph_device_t* graph_devices_index(const graph_devices_s *restrict gds, uint32_t index);

const char* graph_physical_device_type_string(graph_physical_device_type_t type);

void graph_device_properties_dump(const graph_device_t *restrict gd, mlog_s *ml);
void graph_device_features_dump(const graph_device_t *restrict gd, mlog_s *ml);
void graph_device_dump(const graph_device_t *restrict gd, mlog_s *ml);

#endif
