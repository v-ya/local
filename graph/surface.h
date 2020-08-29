#ifndef _graph_surface_h_
#define _graph_surface_h_

#include <refer.h>
#include "type.h"

typedef struct graph_surface_s graph_surface_s;
typedef struct graph_surface_attr_s graph_surface_attr_s;
typedef struct graph_swapchain_param_s graph_swapchain_param_s;
typedef struct graph_swapchain_s graph_swapchain_s;

struct graph_s;
struct graph_device_t;
struct graph_dev_s;
struct graph_semaphore_s;
struct graph_fence_s;

graph_surface_s* graph_surface_xcb_create_window(struct graph_s *restrict g, graph_surface_s *restrict parent, int x, int y, unsigned int width, unsigned int height, unsigned int depth);

graph_surface_attr_s* graph_surface_attr_get(const graph_surface_s *restrict surface, const struct graph_device_t *restrict device);
graph_swapchain_param_s* graph_swapchain_param_alloc(graph_surface_s *restrict surface, const graph_surface_attr_s *restrict attr, uint32_t queue_sharing_number);
graph_swapchain_s* graph_swapchain_alloc(const graph_swapchain_param_s *restrict param, struct graph_dev_s *dev);
uint32_t graph_swapchain_image_number(const graph_swapchain_s *restrict swapchain);
graph_format_t graph_swapchain_format(const graph_swapchain_s *restrict swapchain);
void graph_swapchain_info(const graph_swapchain_s *restrict swapchain, uint32_t *restrict image_number, graph_format_t *restrict format, uint32_t *restrict width, uint32_t *restrict height);
uint32_t graph_swapchain_acquire(graph_swapchain_s *restrict swapchain, uint64_t timeout, struct graph_semaphore_s *restrict semaphore, struct graph_fence_s *restrict fence);

void graph_surface_attr_dump(const graph_surface_attr_s *restrict attr);

#endif
