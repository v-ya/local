#ifndef _iyii_graph_h_
#define _iyii_graph_h_

#include <graph/graph.h>
#include <graph/device.h>
#include <graph/surface.h>
#include <graph/support.h>

graph_s* iyii_graph_create_graph(mlog_s *restrict mlog, const char *restrict app_name, const char *restrict engine_name, uint32_t enable_validation);
graph_device_t* iyii_graph_select_device(const graph_devices_s *devices);

#endif
