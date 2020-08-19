#ifndef _graph_surface_pri_h_
#define _graph_surface_pri_h_

#include "surface.h"
#include <vulkan/vulkan.h>
#include "graph_pri.h"

struct graph_surface_s {
	graph_s *g;
	VkSurfaceKHR surface;
};

#endif
