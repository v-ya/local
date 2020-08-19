#ifndef _graph_surface_h_
#define _graph_surface_h_

#include <refer.h>

typedef struct graph_surface_s graph_surface_s;

struct graph_s;

graph_surface_s* graph_surface_xcb_create_window(struct graph_s *restrict g, graph_surface_s *restrict parent, int x, int y, unsigned int width, unsigned int height, unsigned int depth);

#endif
