#include "xwindow.private.h"
#include "xwindow.atom.h"

static xcb_event_mask_t xwindow_event_inner_get_mask(const xwindow_event_t *restrict events)
{
	xcb_event_mask_t emask;
	emask = 0;
	if (events) while (*events)
	{
		switch (*events)
		{
			case xwindow_event_expose:
				emask |= XCB_EVENT_MASK_EXPOSURE;
				break;
			case xwindow_event_key:
				emask |= XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;
				break;
			case xwindow_event_button:
				emask |= XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE;
				break;
			case xwindow_event_pointer:
				emask |= XCB_EVENT_MASK_POINTER_MOTION |
					XCB_EVENT_MASK_BUTTON_1_MOTION |
					XCB_EVENT_MASK_BUTTON_2_MOTION |
					XCB_EVENT_MASK_BUTTON_3_MOTION |
					XCB_EVENT_MASK_BUTTON_4_MOTION |
					XCB_EVENT_MASK_BUTTON_5_MOTION |
					XCB_EVENT_MASK_BUTTON_MOTION;
				break;
			case xwindow_event_area:
				emask |= XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW;
				break;
			case xwindow_event_focus:
				emask |= XCB_EVENT_MASK_FOCUS_CHANGE;
				break;
			case xwindow_event_config:
				emask |= XCB_EVENT_MASK_STRUCTURE_NOTIFY;
				break;
			default:
				break;
		}
		++events;
	}
	return emask;
}

static xwindow_event_s* xwindow_event_inner_used_mask(xwindow_event_s *restrict r, xcb_event_mask_t emask)
{
	xwindow_s *restrict xwindow;
	xcb_generic_error_t *restrict error;
	uint32_t value;
	xwindow = r->xwindow;
	value = (uint32_t) emask;
	if (!(error = xcb_request_check(xwindow->connection, xcb_change_window_attributes_checked(
			xwindow->connection, xwindow->window, XCB_CW_EVENT_MASK, &value))))
	{
		r->emask = emask;
		return r;
	}
	free(error);
	return NULL;
}

static void xwindow_event_inner_clear_register(xwindow_event_s *restrict r)
{
	#define d_clear(_name)  if (r->data_##_name) { refer_free(r->data_##_name); r->data_##_name = NULL; } r->func_##_name = NULL
	d_clear(close);
	d_clear(expose);
	d_clear(key);
	d_clear(button);
	d_clear(pointer);
	d_clear(area);
	d_clear(focus);
	d_clear(config);
	#undef d_clear
}

static void xwindow_event_free_func(xwindow_event_s *restrict r)
{
	if (r->xwindow) refer_free(r->xwindow);
	if (r->read) refer_free(r->read);
	if (r->write) refer_free(r->write);
	xwindow_event_inner_clear_register(r);
}

xwindow_event_s* xwindow_event_alloc(xwindow_s *restrict xwindow, const xwindow_event_t *restrict events)
{
	xwindow_event_s *restrict r;
	if ((r = (xwindow_event_s *) refer_alloz(sizeof(xwindow_event_s))))
	{
		refer_set_free(r, (refer_free_f) xwindow_event_free_func);
		r->xwindow = (xwindow_s *) refer_save(xwindow);
		if (!yaw_lock_alloc_rwlock(&r->read, &r->write) &&
			(!events || xwindow_event_inner_used_mask(r, xwindow_event_inner_get_mask(events))) &&
			xwindow_atom_get(xwindow->atom, xatom__WM_DELETE_WINDOW, &r->wm_delete_window, xwindow->connection))
			return r;
		refer_free(r);
	}
	return NULL;
}

xwindow_event_s* xwindow_event_used(xwindow_event_s *restrict r, const xwindow_event_t *restrict events)
{
	yaw_lock_s *restrict lock;
	lock = r->write;
	yaw_lock_lock(lock);
	r = xwindow_event_inner_used_mask(r, xwindow_event_inner_get_mask(events));
	yaw_lock_unlock(lock);
	return r;
}

void xwindow_event_clear_register(xwindow_event_s *restrict r)
{
	yaw_lock_s *restrict lock;
	lock = r->write;
	yaw_lock_lock(lock);
	xwindow_event_inner_clear_register(r);
	yaw_lock_unlock(lock);
}

#define xwindow_register_event$def(_name) \
	void xwindow_event_register_##_name(xwindow_event_s *restrict r, xwindow_event_##_name##_f func, refer_t data)\
	{\
		yaw_lock_s *restrict lock;\
		lock = r->write;\
		yaw_lock_lock(lock);\
		if (r->data_##_name) refer_free(r->data_##_name);\
		r->data_##_name = refer_save(data);\
		r->func_##_name = func;\
		yaw_lock_unlock(lock);\
	}
xwindow_register_event$def(close)
xwindow_register_event$def(expose)
xwindow_register_event$def(key)
xwindow_register_event$def(button)
xwindow_register_event$def(pointer)
xwindow_register_event$def(area)
xwindow_register_event$def(focus)
xwindow_register_event$def(config)
#undef xwindow_register_event$def

static void xwindow_event_inner_route(register xwindow_event_s *restrict r, register xcb_generic_event_t *restrict e)
{
	register void *restrict func;
	xwindow_event_state_t state;
	switch (e->response_type & 0x7f)
	{
		case XCB_KEY_PRESS:
		case XCB_KEY_RELEASE:
			if ((func = r->func_key))
			{
				state.x = ((xcb_key_press_event_t *) e)->event_x;
				state.y = ((xcb_key_press_event_t *) e)->event_y;
				state.root_x = ((xcb_key_press_event_t *) e)->root_x;
				state.root_y = ((xcb_key_press_event_t *) e)->root_y;
				state.state = ((xcb_key_press_event_t *) e)->state;
				((xwindow_event_key_f) func)(
					r->data_key, r->xwindow,
					((xcb_key_press_event_t *) e)->detail,
					(e->response_type & 0x7f) == XCB_KEY_PRESS,
					&state
				);
			}
			break;
		case XCB_BUTTON_PRESS:
		case XCB_BUTTON_RELEASE:
			if ((func = r->func_button))
			{
				state.x = ((xcb_button_press_event_t *) e)->event_x;
				state.y = ((xcb_button_press_event_t *) e)->event_y;
				state.root_x = ((xcb_button_press_event_t *) e)->root_x;
				state.root_y = ((xcb_button_press_event_t *) e)->root_y;
				state.state = ((xcb_button_press_event_t *) e)->state;
				((xwindow_event_button_f) func)(
					r->data_button, r->xwindow,
					((xcb_button_press_event_t *) e)->detail,
					(e->response_type & 0x7f) == XCB_BUTTON_PRESS,
					&state
				);
			}
			break;
		case XCB_MOTION_NOTIFY:
			if ((func = r->func_pointer))
			{
				state.x = ((xcb_motion_notify_event_t *) e)->event_x;
				state.y = ((xcb_motion_notify_event_t *) e)->event_y;
				state.root_x = ((xcb_motion_notify_event_t *) e)->root_x;
				state.root_y = ((xcb_motion_notify_event_t *) e)->root_y;
				state.state = ((xcb_motion_notify_event_t *) e)->state;
				((xwindow_event_pointer_f) func)(
					r->data_pointer, r->xwindow,
					&state
				);
			}
			break;
		case XCB_ENTER_NOTIFY:
		case XCB_LEAVE_NOTIFY:
			if ((func = r->func_area) && !((xcb_enter_notify_event_t *) e)->mode)
			{
				state.x = ((xcb_enter_notify_event_t *) e)->event_x;
				state.y = ((xcb_enter_notify_event_t *) e)->event_y;
				state.root_x = ((xcb_enter_notify_event_t *) e)->root_x;
				state.root_y = ((xcb_enter_notify_event_t *) e)->root_y;
				state.state = ((xcb_enter_notify_event_t *) e)->state;
				((xwindow_event_area_f) func)(
					r->data_area, r->xwindow,
					&state
				);
			}
			break;
		case XCB_FOCUS_IN:
		case XCB_FOCUS_OUT:
			if ((func = r->func_focus))
			{
				((xwindow_event_focus_f) func)(
					r->data_focus, r->xwindow,
					(e->response_type & 0x7f) == XCB_FOCUS_IN
				);
			}
			break;
		case XCB_EXPOSE:
			if ((func = r->func_expose))
			{
				((xwindow_event_expose_f) func)(
					r->data_expose, r->xwindow,
					((xcb_expose_event_t *) e)->x,
					((xcb_expose_event_t *) e)->y,
					((xcb_expose_event_t *) e)->width,
					((xcb_expose_event_t *) e)->height
				);
			}
			break;
		case XCB_CONFIGURE_NOTIFY:
			if ((func = r->func_config))
			{
				((xwindow_event_config_f) func)(
					r->data_config, r->xwindow,
					((xcb_configure_notify_event_t *) e)->x,
					((xcb_configure_notify_event_t *) e)->y,
					((xcb_configure_notify_event_t *) e)->width,
					((xcb_configure_notify_event_t *) e)->height
				);
			}
			break;
		case XCB_CLIENT_MESSAGE:
			if (((xcb_client_message_event_t *) e)->data.data32[0] == r->wm_delete_window)
			{
				if ((func = r->func_close))
					((xwindow_event_close_f) func)(r->data_close, r->xwindow);
			}
			break;
	}
}

xwindow_event_s* xwindow_event_do(xwindow_event_s *restrict r)
{
	xcb_generic_event_t *restrict e;
	yaw_lock_s *restrict lock;
	lock = r->read;
	yaw_lock_lock(lock);
	e = xcb_poll_for_event(r->xwindow->connection);
	if (e)
	{
		xwindow_event_inner_route(r, e);
		free(e);
	}
	yaw_lock_unlock(lock);
	return e?r:NULL;
}

uintptr_t xwindow_event_done(xwindow_event_s *restrict r)
{
	xcb_connection_t *restrict connection;
	xcb_generic_event_t *restrict e;
	yaw_lock_s *restrict lock;
	uintptr_t n;
	connection = r->xwindow->connection;
	lock = r->read;
	n = 0;
	yaw_lock_lock(lock);
	while ((e = xcb_poll_for_event(connection)))
	{
		xwindow_event_inner_route(r, e);
		free(e);
		n += 1;
	}
	yaw_lock_unlock(lock);
	return n;
}
