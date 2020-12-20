#ifndef _iyii_graph_h_
#define _iyii_graph_h_

#include <graph/graph.h>
#include <graph/device.h>
#include <graph/surface.h>
#include <graph/image.h>
#include <graph/support.h>

graph_s* iyii_graph_create_graph(mlog_s *restrict mlog, const char *restrict app_name, const char *restrict engine_name, uint32_t enable_validation);
const graph_device_t* iyii_graph_select_device(const graph_devices_s *restrict devices, const graph_surface_s *restrict surface);
graph_dev_s* iyii_graph_create_device(const graph_s *restrict graph, const graph_device_t *restrict device, const graph_device_queue_t *restrict graphics, const graph_device_queue_t *restrict transfer, const graph_device_queue_t *restrict compute);
graph_swapchain_s* iyii_graph_create_swapchain(graph_dev_s *restrict dev, const graph_device_t *restrict device, graph_surface_s *restrict surface);

#endif
