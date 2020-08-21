#include "surface_pri.h"
#include <xcb/xcb.h>
#include <vulkan/vulkan_xcb.h>

typedef struct graph_surface_xcb_s {
	graph_surface_s surface;
	xcb_connection_t *connect;
	xcb_window_t winid;
} graph_surface_xcb_s;

static void graph_surface_xcb_free_func(register graph_surface_xcb_s *restrict r)
{
	graph_surface_uini(&r->surface);
	if (r->connect)
	{
		if (r->winid)
			xcb_destroy_window(r->connect, r->winid);
		xcb_flush(r->connect);
		xcb_disconnect(r->connect);
	}
}

graph_surface_s* graph_surface_xcb_create_window(struct graph_s *restrict g, graph_surface_s *restrict parent, int x, int y, unsigned int width, unsigned int height, unsigned int depth)
{
	register graph_surface_xcb_s *restrict r;
	PFN_vkCreateXcbSurfaceKHR func;
	xcb_screen_t *screen;
	VkXcbSurfaceCreateInfoKHR info;
	int rflush;
	if (graph_surface_init_check(g) &&
		(!parent || refer_get_free(parent) == (refer_free_f) graph_surface_xcb_free_func) &&
		width && height)
	{
		func = (PFN_vkCreateXcbSurfaceKHR) vkGetInstanceProcAddr(g->instance, "vkCreateXcbSurfaceKHR");
		if (func)
		{
			r = (graph_surface_xcb_s *) refer_alloz(sizeof(graph_surface_xcb_s));
			if (r)
			{
				refer_set_free(r, (refer_free_f) graph_surface_xcb_free_func);
				r->surface.g = (graph_s *) refer_save(g);
				r->connect = xcb_connect(NULL, NULL);
				if (!r->connect) goto label_connect;
				screen = (xcb_screen_t *) xcb_setup_roots_iterator(xcb_get_setup(r->connect)).data;
				if (!screen) goto label_screen;
				r->winid = xcb_generate_id(r->connect);
				xcb_create_window(r->connect, depth, r->winid, screen->root,
					(int16_t) x, (int16_t) y, (uint16_t) width, (uint16_t) height, 0,
					XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, 0, NULL);
				xcb_map_window(r->connect, r->winid);
				if ((rflush = xcb_flush(r->connect)) <= 0) goto label_flush;
				// set info
				info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
				info.pNext = NULL;
				info.flags = 0;
				info.connection = r->connect;
				info.window = r->winid;
				if (graph_surface_init(&r->surface, g, (graph_surface_vk_create_f) func, &info))
					return &r->surface;
				label_free:
				refer_free(r);
			}
		}
		else mlog_printf(g->ml, "[graph_surface_xcb_create_window] vkGetInstanceProcAddr(\"vkCreateXcbSurfaceKHR\") fail\n");
	}
	return NULL;
	label_connect:
	mlog_printf(g->ml, "[graph_surface_xcb_create_window] xcb_connect fail\n");
	goto label_free;
	label_screen:
	mlog_printf(g->ml, "[graph_surface_xcb_create_window] get screen fail\n");
	goto label_free;
	label_flush:
	mlog_printf(g->ml, "[graph_surface_xcb_create_window] xcb_flush = %d\n", rflush);
	goto label_free;
}
