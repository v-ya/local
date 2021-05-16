#define _DEFAULT_SOURCE
#include "window.h"
#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// xcb

typedef struct xproto__MOTIF_WM_HINTS_t {
	uint32_t flags;
	uint32_t functions;
	uint32_t decorations;
	int32_t input_mode;
	uint32_t status;
} xproto__MOTIF_WM_HINTS_t;

static xcb_connection_t* window_xcb_get_atom(xcb_connection_t *restrict c, const char *restrict name, xcb_atom_t *restrict atom)
{
	xcb_intern_atom_reply_t *restrict ratom;
	ratom = xcb_intern_atom_reply(c, xcb_intern_atom(c, 1, strlen(name), name), NULL);
	if (ratom)
	{
		*atom = ratom->atom;
		free(ratom);
		return c;
	}
	return NULL;
}

static const xcb_screen_t* window_xcb_get_screen(xcb_connection_t *restrict c, uint32_t depth, xcb_visualid_t *restrict visual)
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

static xcb_connection_t* window_xcb_set_atom_close(xcb_connection_t *restrict c, xcb_window_t window, xcb_atom_t *restrict atom_close)
{
	const char s_protocols[] = "WM_PROTOCOLS";
	const char s_delete_window[] = "WM_DELETE_WINDOW";
	xcb_intern_atom_reply_t *restrict r_protocols;
	xcb_intern_atom_reply_t *restrict r_delete_window;
	xcb_generic_error_t *restrict error;
	xcb_intern_atom_cookie_t c_protocols, c_delete_window;
	c_protocols = xcb_intern_atom(c, 1, sizeof(s_protocols) - 1, s_protocols);
	c_delete_window = xcb_intern_atom(c, 1, sizeof(s_delete_window) - 1, s_delete_window);
	r_protocols = xcb_intern_atom_reply(c, c_protocols, NULL);
	r_delete_window = xcb_intern_atom_reply(c, c_delete_window, NULL);
	if (r_protocols && r_delete_window)
	{
		*atom_close = r_delete_window->atom;
		if ((error = xcb_request_check(c, xcb_change_property_checked(
				c, XCB_PROP_MODE_REPLACE, window,
				r_protocols->atom, XCB_ATOM_ATOM, 32, 1, atom_close))))
		{
			free(error);
			goto label_fail;
		}
	}
	else
	{
		label_fail:
		c = NULL;
	}
	if (r_protocols) free(r_protocols);
	if (r_delete_window) free(r_delete_window);
	return c;
}

// shm

typedef struct window_shm_s {
	void *addr;
	uintptr_t size;
	uint32_t shmid;
	uint32_t remove;
} window_shm_s;

static void window_shm_free_func(window_shm_s *restrict r)
{
	if (~(uintptr_t) r->addr)
		shmdt(r->addr);
	if (r->remove)
		shmctl(r->shmid, IPC_RMID, NULL);
}

window_shm_s* window_shm_alloc(uintptr_t size)
{
	window_shm_s *restrict r;
	int shmid;
	if ((size = (size + 4095) & ~4095))
	{
		r = (window_shm_s *) refer_alloz(sizeof(window_shm_s));
		if (r)
		{
			refer_set_free(r, (refer_free_f) window_shm_free_func);
			r->addr = (void *) ~(intptr_t) 0;
			r->size = size;
			shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);
			if (shmid >= 0)
			{
				r->shmid = (uint32_t) shmid;
				r->remove = 1;
				if (~(uintptr_t) (r->addr = shmat(shmid, NULL, 0)))
					return r;
			}
			return r;
			refer_free(r);
		}
	}
	return NULL;
}

// window

typedef struct window_event_report_t {
	refer_t data;
	window_event_close_f   do_close;
	window_event_expose_f  do_expose;
	window_event_key_f     do_key;
	window_event_button_f  do_button;
	window_event_pointer_f do_pointer;
	window_event_area_f    do_area;
	window_event_focus_f   do_focus;
	window_event_config_f  do_config;
} window_event_report_t;

struct window_s {
	xcb_connection_t *connection;
	const xcb_screen_t *screen;
	window_shm_s *shm;
	xcb_visualid_t visual;
	xcb_colormap_t colormap;
	xcb_window_t window;
	xcb_gcontext_t gcontext;
	xcb_shm_seg_t shmseg;
	window_event_report_t report;
	uint32_t depth;
	uint32_t max_request_length;
	xcb_atom_t atom_close;
	xcb_atom_t atom_hint;
	xcb_atom_t atom_state;
	xcb_atom_t atom_state_fullscreen;
};

void window_usleep(uint32_t us)
{
	usleep(us);
}

static void window_free_func(window_s *restrict r)
{
	if (r->connection)
	{
		if (r->shmseg)
			xcb_shm_detach(r->connection, r->shmseg);
		if (r->gcontext)
			xcb_free_gc(r->connection, r->gcontext);
		if (r->window)
			xcb_destroy_window(r->connection, r->window);
		if (r->colormap)
			xcb_free_colormap(r->connection, r->colormap);
		xcb_flush(r->connection);
		xcb_disconnect(r->connection);
	}
	if (r->shm)
		refer_free(r->shm);
	if (r->report.data)
		refer_free(r->report.data);
}

window_s* window_alloc(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t depth)
{
	window_s *restrict r;
	xcb_generic_error_t *restrict error;
	r = (window_s *) refer_alloz(sizeof(window_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) window_free_func);
		error = NULL;
		r->connection = xcb_connect(NULL, NULL);
		if (!r->connection)
			goto label_fail;
		r->screen = window_xcb_get_screen(r->connection, depth, &r->visual);
		if (!r->screen)
			goto label_fail;
		r->colormap = xcb_generate_id(r->connection);
		if ((error = xcb_request_check(r->connection, xcb_create_colormap_checked(
				r->connection, XCB_COLORMAP_ALLOC_NONE,
				r->colormap, r->screen->root, r->visual))))
			goto label_fail;
		r->window = xcb_generate_id(r->connection);
		if ((error = xcb_request_check(r->connection, xcb_create_window_checked(
				r->connection, (uint8_t) depth, r->window, r->screen->root,
				(int16_t) x, (int16_t) y, (uint16_t) w, (uint16_t) h, 0,
				XCB_WINDOW_CLASS_INPUT_OUTPUT, r->visual,
				XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_COLORMAP,
				(uint32_t []) {0, 0, r->colormap}))))
			goto label_fail;
		if (!window_xcb_set_atom_close(r->connection, r->window, &r->atom_close))
			goto label_fail;
		r->gcontext = xcb_generate_id(r->connection);
		if ((error = xcb_request_check(r->connection, xcb_create_gc_checked(
				r->connection, r->gcontext, r->window, 0, NULL))))
			goto label_fail;
		r->depth = depth?depth:r->screen->root_depth;
		r->max_request_length = xcb_get_maximum_request_length(r->connection);
		if (r->max_request_length > 1024) r->max_request_length -= 1024;
		else r->max_request_length = 0;
		if (xcb_flush(r->connection) <= 0)
			goto label_fail;
		return r;
		label_fail:
		if (error) free(error);
		refer_free(r);
	}
	return NULL;
}

window_s* window_enable_shm(window_s *restrict r, uintptr_t shm_size)
{
	xcb_generic_error_t *restrict error;
	window_shm_s *restrict shm;
	xcb_shm_seg_t shmseg;
	shm = NULL;
	if (r->shm)
		goto label_fail;
	if (!(shm = window_shm_alloc(shm_size)))
		goto label_fail;
	shmseg = xcb_generate_id(r->connection);
	if (!(error = xcb_request_check(r->connection, xcb_shm_attach_checked(
			r->connection, shmseg, shm->shmid, 1))))
	{
		r->shm = shm;
		r->shmseg = shmseg;
		return r;
	}
	free(error);
	label_fail:
	if (shm) refer_free(shm);
	return NULL;
}

window_s* window_disable_shm(window_s *restrict r)
{
	if (r->shm)
	{
		if (r->shmseg)
		{
			xcb_shm_detach(r->connection, r->shmseg);
			r->shmseg = 0;
		}
		refer_free(r->shm);
		r->shm = NULL;
		return r;
	}
	return NULL;
}

window_s* window_map(window_s *restrict r)
{
	xcb_generic_error_t *restrict error;
	if (!(error = xcb_request_check(r->connection, xcb_map_window_checked(r->connection, r->window))))
	{
		if (xcb_flush(r->connection) <= 0)
			goto label_fail;
		return r;
	}
	free(error);
	label_fail:
	return NULL;
}

window_s* window_unmap(window_s *restrict r)
{
	xcb_generic_error_t *restrict error;
	if (!(error = xcb_request_check(r->connection, xcb_unmap_window_checked(r->connection, r->window))))
	{
		if (xcb_flush(r->connection) <= 0)
			goto label_fail;
		return r;
	}
	free(error);
	label_fail:
	return NULL;
}

window_s* window_update(window_s *restrict r, const uint32_t *restrict data, uint32_t width, uint32_t height, int32_t x, int32_t y)
{
	xcb_generic_error_t *restrict error;
	uint32_t linesize, nl;
	uintptr_t shm_nl;
	if (r->depth != 24 && r->depth != 32)
		goto label_fail;
	linesize = width << 2;
	error = NULL;
	if (r->shm)
	{
		shm_nl = r->shm->size / linesize;
		if (!shm_nl) goto label_fail;
		if ((uintptr_t) (nl = (uint32_t) shm_nl) != shm_nl)
			nl = ~(uint32_t) 0;
		do {
			if (nl > height) nl = height;
			memcpy(r->shm->addr, data, linesize * nl);
			error = xcb_request_check(r->connection, xcb_shm_put_image_checked(
				r->connection, r->window, r->gcontext,
				(uint16_t) width, (uint16_t) nl, 0, 0,
				(uint16_t) width, (uint16_t) nl, (int16_t) x, (int16_t) y,
				r->depth, XCB_IMAGE_FORMAT_Z_PIXMAP, 0, r->shmseg, 0));
			data += width * nl;
			y += nl;
			height -= nl;
		} while (!error && height);
	}
	else
	{
		nl = r->max_request_length / linesize;
		if (!nl) goto label_fail;
		do {
			if (nl > height) nl = height;
			error = xcb_request_check(r->connection, xcb_put_image_checked(
				r->connection, XCB_IMAGE_FORMAT_Z_PIXMAP, r->window, r->gcontext,
				(uint16_t) width, (uint16_t) nl, (int16_t) x, (int16_t) y,
				0, r->depth, linesize * nl, (const uint8_t *) data));
			data += width * nl;
			y += nl;
			height -= nl;
		} while (!error && height);
	}
	if (!error) return r;
	free(error);
	label_fail:
	return NULL;
}

window_s* window_set_event(window_s *restrict r, const window_event_t *restrict events)
{
	xcb_generic_error_t *error;
	uint32_t value;
	value = 0;
	if (events) while (*events)
	{
		switch (*events)
		{
			case window_event_expose:
				value |= XCB_EVENT_MASK_EXPOSURE;
				break;
			case window_event_key:
				value |= XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;
				break;
			case window_event_button:
				value |= XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE;
				break;
			case window_event_pointer:
				value |= XCB_EVENT_MASK_POINTER_MOTION |
					XCB_EVENT_MASK_BUTTON_1_MOTION |
					XCB_EVENT_MASK_BUTTON_2_MOTION |
					XCB_EVENT_MASK_BUTTON_3_MOTION |
					XCB_EVENT_MASK_BUTTON_4_MOTION |
					XCB_EVENT_MASK_BUTTON_5_MOTION |
					XCB_EVENT_MASK_BUTTON_MOTION;
				break;
			case window_event_area:
				value |= XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW;
				break;
			case window_event_focus:
				value |= XCB_EVENT_MASK_FOCUS_CHANGE;
				break;
			case window_event_config:
				value |= XCB_EVENT_MASK_STRUCTURE_NOTIFY;
				break;
			default:
				break;
		}
		++events;
	}
	if (!(error = xcb_request_check(r->connection, xcb_change_window_attributes_checked(
			r->connection, r->window, XCB_CW_EVENT_MASK, &value))))
		return r;
	free(error);
	return NULL;
}

window_s* window_do_event(register window_s *restrict r)
{
	register xcb_generic_event_t *restrict e;
	register void *restrict func;
	window_event_state_t state;
	e = xcb_poll_for_event(r->connection);
	if (e)
	{
		switch (e->response_type & 0x7f)
		{
			case XCB_KEY_PRESS:
			case XCB_KEY_RELEASE:
				if ((func = r->report.do_key))
				{
					state.x = ((xcb_key_press_event_t *) e)->event_x;
					state.y = ((xcb_key_press_event_t *) e)->event_y;
					state.root_x = ((xcb_key_press_event_t *) e)->root_x;
					state.root_y = ((xcb_key_press_event_t *) e)->root_y;
					state.state = ((xcb_key_press_event_t *) e)->state;
					((window_event_key_f) func)(
						r->report.data, r,
						((xcb_key_press_event_t *) e)->detail,
						(e->response_type & 0x7f) == XCB_KEY_PRESS,
						&state
					);
				}
				break;
			case XCB_BUTTON_PRESS:
			case XCB_BUTTON_RELEASE:
				if ((func = r->report.do_button))
				{
					state.x = ((xcb_button_press_event_t *) e)->event_x;
					state.y = ((xcb_button_press_event_t *) e)->event_y;
					state.root_x = ((xcb_button_press_event_t *) e)->root_x;
					state.root_y = ((xcb_button_press_event_t *) e)->root_y;
					state.state = ((xcb_button_press_event_t *) e)->state;
					((window_event_button_f) func)(
						r->report.data, r,
						((xcb_button_press_event_t *) e)->detail,
						(e->response_type & 0x7f) == XCB_BUTTON_PRESS,
						&state
					);
				}
				break;
			case XCB_MOTION_NOTIFY:
				if ((func = r->report.do_pointer))
				{
					state.x = ((xcb_motion_notify_event_t *) e)->event_x;
					state.y = ((xcb_motion_notify_event_t *) e)->event_y;
					state.root_x = ((xcb_motion_notify_event_t *) e)->root_x;
					state.root_y = ((xcb_motion_notify_event_t *) e)->root_y;
					state.state = ((xcb_motion_notify_event_t *) e)->state;
					((window_event_pointer_f) func)(
						r->report.data, r,
						&state
					);
				}
				break;
			case XCB_ENTER_NOTIFY:
			case XCB_LEAVE_NOTIFY:
				if ((func = r->report.do_area) && !((xcb_enter_notify_event_t *) e)->mode)
				{
					state.x = ((xcb_enter_notify_event_t *) e)->event_x;
					state.y = ((xcb_enter_notify_event_t *) e)->event_y;
					state.root_x = ((xcb_enter_notify_event_t *) e)->root_x;
					state.root_y = ((xcb_enter_notify_event_t *) e)->root_y;
					state.state = ((xcb_enter_notify_event_t *) e)->state;
					((window_event_area_f) func)(
						r->report.data, r,
						&state
					);
				}
				break;
			case XCB_FOCUS_IN:
			case XCB_FOCUS_OUT:
				if ((func = r->report.do_focus))
				{
					((window_event_focus_f) func)(
						r->report.data, r,
						(e->response_type & 0x7f) == XCB_FOCUS_IN
					);
				}
				break;
			case XCB_EXPOSE:
				if ((func = r->report.do_expose))
				{
					((window_event_expose_f) func)(
						r->report.data, r,
						((xcb_expose_event_t *) e)->x,
						((xcb_expose_event_t *) e)->y,
						((xcb_expose_event_t *) e)->width,
						((xcb_expose_event_t *) e)->height
					);
				}
				break;
			case XCB_CONFIGURE_NOTIFY:
				if ((func = r->report.do_config))
				{
					((window_event_config_f) func)(
						r->report.data, r,
						((xcb_configure_notify_event_t *) e)->x,
						((xcb_configure_notify_event_t *) e)->y,
						((xcb_configure_notify_event_t *) e)->width,
						((xcb_configure_notify_event_t *) e)->height
					);
				}
				break;
			case XCB_CLIENT_MESSAGE:
				if (((xcb_client_message_event_t *) e)->data.data32[0] == r->atom_close)
				{
					if ((func = r->report.do_close))
						((window_event_close_f) func)(r->report.data, r);
				}
				break;
		}
		free(e);
	}
	return e?r:NULL;
}

uintptr_t window_do_all_events(window_s *restrict r)
{
	uintptr_t n = 0;
	while (window_do_event(r))
		++n;
	return n;
}

void window_register_event_data(window_s *restrict r, refer_t data)
{
	if (r->report.data)
		refer_free(r->report.data);
	r->report.data = refer_save(data);
}

#define window_register_event$def(_name) \
	void window_register_event_##_name(register window_s *restrict r, register window_event_##_name##_f func)\
	{r->report.do_##_name = func;}
window_register_event$def(close)
window_register_event$def(expose)
window_register_event$def(key)
window_register_event$def(button)
window_register_event$def(pointer)
window_register_event$def(area)
window_register_event$def(focus)
window_register_event$def(config)
#undef window_register_event$def

void window_register_clear(window_s *restrict r)
{
	if (r->report.data)
		refer_free(r->report.data);
	memset(&r->report, 0, sizeof(r->report));
}

const window_s* window_get_screen_size(const window_s *restrict r, uint32_t *restrict width_pixels, uint32_t *restrict height_pixels, uint32_t *restrict width_mm, uint32_t *restrict height_mm, uint32_t *restrict depth)
{
	const xcb_screen_t *restrict screen;
	if ((screen = r->screen))
	{
		if (width_pixels) *width_pixels = screen->width_in_pixels;
		if (height_pixels) *height_pixels = screen->height_in_pixels;
		if (width_mm) *width_mm = screen->width_in_millimeters;
		if (height_mm) *height_mm = screen->height_in_millimeters;
		if (depth) *depth = screen->root_depth;
		return r;
	}
	return NULL;
}

const window_s* window_get_geometry(const window_s *restrict r, uint32_t *restrict width, uint32_t *restrict height, int32_t *restrict x, int32_t *restrict y, uint32_t *restrict depth)
{
	xcb_get_geometry_reply_t *restrict geometry;
	geometry = xcb_get_geometry_reply(r->connection, xcb_get_geometry(r->connection, r->window), NULL);
	if (geometry)
	{
		if (width) *width = geometry->width;
		if (height) *height = geometry->height;
		if (x) *x = geometry->x;
		if (y) *y = geometry->y;
		if (depth) *depth = geometry->depth;
		free(geometry);
		return r;
	}
	return NULL;
}

window_s* window_set_position(window_s *restrict r, int32_t x, int32_t y)
{
	xcb_generic_error_t *error;
	if (!(error = xcb_request_check(r->connection, xcb_configure_window_checked(
			r->connection, r->window,
			XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
			(const uint32_t [2]) {x, y}))))
		return r;
	free(error);
	return NULL;
}

window_s* window_set_size(window_s *restrict r, uint32_t width, uint32_t height)
{
	xcb_generic_error_t *error;
	if (!(error = xcb_request_check(r->connection, xcb_configure_window_checked(
			r->connection, r->window,
			XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
			(const uint32_t [2]) {width, height}))))
		return r;
	free(error);
	return NULL;
}

window_s* window_set_hint_decorations(window_s *restrict r, uint32_t enable)
{
	xcb_generic_error_t *error;
	if (!r->atom_hint)
	{
		if (!window_xcb_get_atom(r->connection, "_MOTIF_WM_HINTS", &r->atom_hint))
			goto label_fail;
	}
	if (!(error = xcb_request_check(r->connection, xcb_change_property_checked(
			r->connection, XCB_PROP_MODE_REPLACE, r->window,
			r->atom_hint, r->atom_hint, 32,
			sizeof(xproto__MOTIF_WM_HINTS_t) / sizeof(uint32_t),
			(xproto__MOTIF_WM_HINTS_t [1]) {{
				.flags = 2,
				.functions = 0,
				.decorations = !!enable,
				.input_mode = 0,
				.status = 0
			}}))))
		return r;
	free(error);
	label_fail:
	return NULL;
}

window_s* window_set_fullscreen(window_s *restrict r, uint32_t enable)
{
	xcb_generic_error_t *error;
	if (!r->atom_state)
	{
		if (!window_xcb_get_atom(r->connection, "_NET_WM_STATE", &r->atom_state))
			goto label_fail;
	}
	if (!r->atom_state_fullscreen)
	{
		if (!window_xcb_get_atom(r->connection, "_NET_WM_STATE_FULLSCREEN", &r->atom_state_fullscreen))
			goto label_fail;
	}
	error = xcb_request_check(r->connection, xcb_send_event_checked(
		r->connection, 0, r->screen->root,
		XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
		(char *) (xcb_client_message_event_t [1]) {{
			.response_type = XCB_CLIENT_MESSAGE,
			.format = 32,
			.sequence = 0,
			.window = r->window,
			.type = r->atom_state,
			.data = {.data32 = {enable?1:0, r->atom_state_fullscreen, 0, 0, 0}}
		}})
	);
	if (!error) return r;
	free(error);
	label_fail:
	return NULL;
}
