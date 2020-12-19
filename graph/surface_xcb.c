#include "surface_pri.h"
#include <xcb/xcb.h>
#include <vulkan/vulkan_xcb.h>
#include <stdlib.h>

typedef struct graph_surface_xcb_s {
	graph_surface_s surface;
	xcb_connection_t *connect;
	xcb_colormap_t colormap;
	xcb_window_t winid;
	xcb_atom_t atom_close;
} graph_surface_xcb_s;

static const graph_surface_s* graph_surface_xcb_do_event_func(register graph_surface_xcb_s *restrict r)
{
	register xcb_generic_event_t *restrict e;
	register void *restrict func;
	graph_surface_do_event_state_t state;
	e = xcb_poll_for_event(r->connect);
	if (e)
	{
		switch (e->response_type & 0x7f)
		{
			case XCB_KEY_PRESS:
			case XCB_KEY_RELEASE:
				if ((func = r->surface.report.do_key))
				{
					state.x = ((xcb_key_press_event_t *) e)->event_x;
					state.y = ((xcb_key_press_event_t *) e)->event_y;
					state.root_x = ((xcb_key_press_event_t *) e)->root_x;
					state.root_y = ((xcb_key_press_event_t *) e)->root_y;
					state.state = ((xcb_key_press_event_t *) e)->state;
					((graph_surface_do_event_key_f) func)(
						r->surface.report.data, &r->surface,
						((xcb_key_press_event_t *) e)->detail,
						(e->response_type & 0x7f) == XCB_KEY_PRESS,
						&state
					);
				}
				break;
			case XCB_BUTTON_PRESS:
			case XCB_BUTTON_RELEASE:
				if ((func = r->surface.report.do_button))
				{
					state.x = ((xcb_button_press_event_t *) e)->event_x;
					state.y = ((xcb_button_press_event_t *) e)->event_y;
					state.root_x = ((xcb_button_press_event_t *) e)->root_x;
					state.root_y = ((xcb_button_press_event_t *) e)->root_y;
					state.state = ((xcb_button_press_event_t *) e)->state;
					((graph_surface_do_event_button_f) func)(
						r->surface.report.data, &r->surface,
						((xcb_button_press_event_t *) e)->detail,
						(e->response_type & 0x7f) == XCB_BUTTON_PRESS,
						&state
					);
				}
				break;
			case XCB_MOTION_NOTIFY:
				if ((func = r->surface.report.do_pointer))
				{
					state.x = ((xcb_motion_notify_event_t *) e)->event_x;
					state.y = ((xcb_motion_notify_event_t *) e)->event_y;
					state.root_x = ((xcb_motion_notify_event_t *) e)->root_x;
					state.root_y = ((xcb_motion_notify_event_t *) e)->root_y;
					state.state = ((xcb_motion_notify_event_t *) e)->state;
					((graph_surface_do_event_pointer_f) func)(
						r->surface.report.data, &r->surface,
						&state
					);
				}
				break;
			case XCB_ENTER_NOTIFY:
			case XCB_LEAVE_NOTIFY:
				if ((func = r->surface.report.do_area) && !((xcb_enter_notify_event_t *) e)->mode)
				{
					state.x = ((xcb_enter_notify_event_t *) e)->event_x;
					state.y = ((xcb_enter_notify_event_t *) e)->event_y;
					state.root_x = ((xcb_enter_notify_event_t *) e)->root_x;
					state.root_y = ((xcb_enter_notify_event_t *) e)->root_y;
					state.state = ((xcb_enter_notify_event_t *) e)->state;
					((graph_surface_do_event_area_f) func)(
						r->surface.report.data, &r->surface,
						&state
					);
				}
				break;
			case XCB_FOCUS_IN:
			case XCB_FOCUS_OUT:
				if ((func = r->surface.report.do_focus))
				{
					((graph_surface_do_event_focus_f) func)(
						r->surface.report.data, &r->surface,
						(e->response_type & 0x7f) == XCB_FOCUS_IN
					);
				}
				break;
			case XCB_EXPOSE:
				if ((func = r->surface.report.do_expose))
				{
					((graph_surface_do_event_expose_f) func)(
						r->surface.report.data, &r->surface,
						((xcb_expose_event_t *) e)->x,
						((xcb_expose_event_t *) e)->y,
						((xcb_expose_event_t *) e)->width,
						((xcb_expose_event_t *) e)->height
					);
				}
				break;
			case XCB_RESIZE_REQUEST:
				if ((func = r->surface.report.do_resize))
				{
					((graph_surface_do_event_resize_f) func)(
						r->surface.report.data, &r->surface,
						((xcb_resize_request_event_t *) e)->width,
						((xcb_resize_request_event_t *) e)->height
					);
				}
				break;
			case XCB_CLIENT_MESSAGE:
				if (((xcb_client_message_event_t *) e)->data.data32[0] == r->atom_close)
				{
					if ((func = r->surface.report.do_close))
						((graph_surface_do_event_close_f) func)(r->surface.report.data, &r->surface);
				}
				break;
		}
		free(e);
	}
	return e?&r->surface:NULL;
}

static const graph_surface_s* graph_surface_xcb_set_event_func(register graph_surface_xcb_s *restrict r, const graph_surface_event_t *restrict events)
{
	register xcb_generic_error_t *error;
	xcb_void_cookie_t cookie;
	uint32_t value;
	value = 0;
	if (events) while (*events)
	{
		switch (*events)
		{
			case graph_surface_event_expose:
				value |= XCB_EVENT_MASK_EXPOSURE;
				break;
			case graph_surface_event_key:
				value |= XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;
				break;
			case graph_surface_event_button:
				value |= XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE;
				break;
			case graph_surface_event_pointer:
				value |= XCB_EVENT_MASK_POINTER_MOTION |
					XCB_EVENT_MASK_BUTTON_1_MOTION |
					XCB_EVENT_MASK_BUTTON_2_MOTION |
					XCB_EVENT_MASK_BUTTON_3_MOTION |
					XCB_EVENT_MASK_BUTTON_4_MOTION |
					XCB_EVENT_MASK_BUTTON_5_MOTION |
					XCB_EVENT_MASK_BUTTON_MOTION;
				break;
			case graph_surface_event_area:
				value |= XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW;
				break;
			case graph_surface_event_focus:
				value |= XCB_EVENT_MASK_FOCUS_CHANGE;
				break;
			case graph_surface_event_resize:
				value |= XCB_EVENT_MASK_RESIZE_REDIRECT;
				break;
			default:
				break;
		}
		++events;
	}
	cookie = xcb_change_window_attributes_checked(r->connect, r->winid, XCB_CW_EVENT_MASK, &value);
	error = xcb_request_check(r->connect, cookie);
	if (!error) return &r->surface;
	free(error);
	return NULL;
}

static const graph_surface_s* graph_surface_xcb_get_geometry_func(register const graph_surface_xcb_s *restrict r, register graph_surface_geometry_t *restrict geometry)
{
	register xcb_get_geometry_reply_t *reply;
	xcb_get_geometry_cookie_t cookie;
	cookie = xcb_get_geometry_unchecked(r->connect, r->winid);
	reply = xcb_get_geometry_reply(r->connect, cookie, NULL);
	if (reply)
	{
		geometry->x = reply->x;
		geometry->y = reply->y;
		geometry->width = reply->width;
		geometry->height = reply->height;
		geometry->depth = reply->depth;
		free(reply);
		return &r->surface;
	}
	return NULL;
}

static const xcb_screen_t* graph_surface_xcb_get_screen(xcb_connection_t *restrict c, uint32_t depth, xcb_visualid_t *restrict visual)
{
	const xcb_setup_t *restrict setup;
	const xcb_screen_t *restrict screen;
	const xcb_depth_t *restrict d;
	const xcb_visualtype_t *vt;
	setup = xcb_get_setup(c);
	if (setup)
	{
		xcb_screen_iterator_t screen_iterator;
		xcb_depth_iterator_t depth_iterator;
		for (screen_iterator = xcb_setup_roots_iterator(setup);
			(screen = screen_iterator.data) && screen_iterator.rem > 0;
			xcb_screen_next(&screen_iterator))
		{
			if (!depth || screen->root_depth == depth)
			{
				*visual = screen->root_visual;
				return screen;
			}
			if (screen->allowed_depths_len)
			{
				for (depth_iterator = xcb_screen_allowed_depths_iterator(screen_iterator.data);
					(d = depth_iterator.data) && depth_iterator.rem > 0;
					xcb_depth_next(&depth_iterator))
				{
					if (d->visuals_len && d->depth == depth && (vt = xcb_depth_visuals(d)))
					{
						*visual = vt->visual_id;
						return screen;
					}
				}
			}
		}
	}
	return NULL;
}

static graph_surface_xcb_s* graph_surface_xcb_set_atom_close(graph_surface_xcb_s *restrict r, mlog_s *restrict ml)
{
	xcb_connection_t *restrict c;
	xcb_intern_atom_reply_t *restrict r_protocols;
	xcb_intern_atom_reply_t *restrict r_delete_window;
	xcb_intern_atom_cookie_t c_protocols, c_delete_window;
	c = r->connect;
	c_protocols = xcb_intern_atom(c, 1, 12, "WM_PROTOCOLS");
	c_delete_window = xcb_intern_atom(c, 1, 16, "WM_DELETE_WINDOW");
	r_protocols = xcb_intern_atom_reply(c, c_protocols, NULL);
	r_delete_window = xcb_intern_atom_reply(c, c_delete_window, NULL);
	if (r_protocols && r_delete_window)
	{
		r->atom_close = r_delete_window->atom;
		xcb_change_property(c, XCB_PROP_MODE_REPLACE, r->winid, r_protocols->atom, 4, 32, 1, &r->atom_close);
	}
	else r = NULL;
	if (r_protocols) free(r_protocols);
	if (r_delete_window) free(r_delete_window);
	return r;
}

static void graph_surface_xcb_free_func(register graph_surface_xcb_s *restrict r)
{
	if (r->connect)
	{
		if (r->winid)
			xcb_destroy_window(r->connect, r->winid);
		if (r->colormap)
			xcb_free_colormap(r->connect, r->colormap);
		xcb_flush(r->connect);
		xcb_disconnect(r->connect);
	}
	graph_surface_uini(&r->surface);
}

graph_surface_s* graph_surface_xcb_create_window(struct graph_s *restrict g, graph_surface_s *restrict parent, int x, int y, unsigned int width, unsigned int height, unsigned int depth)
{
	register graph_surface_xcb_s *restrict r;
	const xcb_screen_t *screen;
	xcb_generic_error_t *error;
	PFN_vkCreateXcbSurfaceKHR func;
	VkXcbSurfaceCreateInfoKHR info;
	xcb_visualid_t visual;
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
				r->connect = xcb_connect(NULL, NULL);
				if (!r->connect) goto label_connect;
				screen = graph_surface_xcb_get_screen(r->connect, depth, &visual);
				if (!screen) goto label_screen;
				r->colormap = xcb_generate_id(r->connect);
				error = xcb_request_check(r->connect,
					xcb_create_colormap_checked(r->connect, XCB_COLORMAP_ALLOC_NONE,
					r->colormap, screen->root, visual));
				if (error) goto label_colormap;
				r->winid = xcb_generate_id(r->connect);
				error = xcb_request_check(r->connect,
					xcb_create_window_checked(r->connect, depth, r->winid, screen->root,
						(int16_t) x, (int16_t) y, (uint16_t) width, (uint16_t) height, 0,
						XCB_WINDOW_CLASS_INPUT_OUTPUT, visual,
						XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_COLORMAP,
						(uint32_t []) {0, 0, r->colormap}));
				if (error) goto label_window;
				if (!graph_surface_xcb_set_atom_close(r, g->ml)) goto label_set;
				error = xcb_request_check(r->connect, xcb_map_window_checked(r->connect, r->winid));
				if (error) goto label_map;
				if ((rflush = xcb_flush(r->connect)) <= 0) goto label_flush;
				// set info
				info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
				info.pNext = NULL;
				info.flags = 0;
				info.connection = r->connect;
				info.window = r->winid;
				if (graph_surface_init(&r->surface, g, (graph_surface_vk_create_f) func, &info))
				{
					r->surface.control.do_event = (graph_surface_do_event_f) graph_surface_xcb_do_event_func;
					r->surface.control.set_event = (graph_surface_set_event_f) graph_surface_xcb_set_event_func;
					r->surface.control.get_geometry = (graph_surface_get_geometry_f) graph_surface_xcb_get_geometry_func;
					return &r->surface;
				}
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
	label_colormap:
	mlog_printf(g->ml, "[graph_surface_xcb_create_window] create colormap fail, error code: %u\n", error->error_code);
	free(error);
	goto label_free;
	label_window:
	mlog_printf(g->ml, "[graph_surface_xcb_create_window] create window fail, error code: %u\n", error->error_code);
	free(error);
	goto label_free;
	label_set:
	mlog_printf(g->ml, "[graph_surface_xcb_create_window] xcb set get atom fail\n");
	goto label_free;
	label_map:
	mlog_printf(g->ml, "[graph_surface_xcb_create_window] map window fail, error code: %u\n", error->error_code);
	free(error);
	goto label_free;
	label_flush:
	mlog_printf(g->ml, "[graph_surface_xcb_create_window] xcb_flush = %d\n", rflush);
	goto label_free;
}
