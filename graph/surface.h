#ifndef _graph_surface_h_
#define _graph_surface_h_

#include <refer.h>
#include "type.h"

typedef struct graph_surface_s graph_surface_s;
typedef struct graph_surface_attr_s graph_surface_attr_s;

struct graph_s;
struct graph_device_t;

graph_surface_s* graph_surface_xcb_create_window(struct graph_s *restrict g, graph_surface_s *restrict parent, int x, int y, unsigned int width, unsigned int height, unsigned int depth);

graph_surface_attr_s* graph_surface_attr_get(const graph_surface_s *restrict surface, const struct graph_device_t *restrict device);
void graph_surface_attr_dump(const graph_surface_attr_s *restrict attr);

#endif
