#include "xwindow.private.h"
#include <memory.h>

xwindow_s* xwindow_set_event(xwindow_s *restrict r, const xwindow_event_t *restrict events)
{
	xcb_generic_error_t *error;
	uint32_t value;
	value = 0;
	if (events) while (*events)
	{
		switch (*events)
		{
			case xwindow_event_expose:
				value |= XCB_EVENT_MASK_EXPOSURE;
				break;
			case xwindow_event_key:
				value |= XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;
				break;
			case xwindow_event_button:
				value |= XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE;
				break;
			case xwindow_event_pointer:
				value |= XCB_EVENT_MASK_POINTER_MOTION |
					XCB_EVENT_MASK_BUTTON_1_MOTION |
					XCB_EVENT_MASK_BUTTON_2_MOTION |
					XCB_EVENT_MASK_BUTTON_3_MOTION |
					XCB_EVENT_MASK_BUTTON_4_MOTION |
					XCB_EVENT_MASK_BUTTON_5_MOTION |
					XCB_EVENT_MASK_BUTTON_MOTION;
				break;
			case xwindow_event_area:
				value |= XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW;
				break;
			case xwindow_event_focus:
				value |= XCB_EVENT_MASK_FOCUS_CHANGE;
				break;
			case xwindow_event_config:
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

xwindow_s* xwindow_do_event(register xwindow_s *restrict r)
{
	register xcb_generic_event_t *restrict e;
	register void *restrict func;
	xwindow_event_state_t state;
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
					((xwindow_event_key_f) func)(
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
					((xwindow_event_button_f) func)(
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
					((xwindow_event_pointer_f) func)(
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
					((xwindow_event_area_f) func)(
						r->report.data, r,
						&state
					);
				}
				break;
			case XCB_FOCUS_IN:
			case XCB_FOCUS_OUT:
				if ((func = r->report.do_focus))
				{
					((xwindow_event_focus_f) func)(
						r->report.data, r,
						(e->response_type & 0x7f) == XCB_FOCUS_IN
					);
				}
				break;
			case XCB_EXPOSE:
				if ((func = r->report.do_expose))
				{
					((xwindow_event_expose_f) func)(
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
					((xwindow_event_config_f) func)(
						r->report.data, r,
						((xcb_configure_notify_event_t *) e)->x,
						((xcb_configure_notify_event_t *) e)->y,
						((xcb_configure_notify_event_t *) e)->width,
						((xcb_configure_notify_event_t *) e)->height
					);
				}
				break;
			case XCB_CLIENT_MESSAGE:
				if (((xcb_client_message_event_t *) e)->data.data32[0] == r->atom.list[xwindow_atom_id__wm_delete_window])
				{
					if ((func = r->report.do_close))
						((xwindow_event_close_f) func)(r->report.data, r);
				}
				break;
		}
		free(e);
	}
	return e?r:NULL;
}

uintptr_t xwindow_do_all_events(xwindow_s *restrict r)
{
	uintptr_t n = 0;
	while (xwindow_do_event(r))
		++n;
	return n;
}

void xwindow_register_event_data(xwindow_s *restrict r, refer_t data)
{
	if (r->report.data)
		refer_free(r->report.data);
	r->report.data = refer_save(data);
}

#define xwindow_register_event$def(_name) \
	void xwindow_register_event_##_name(register xwindow_s *restrict r, register xwindow_event_##_name##_f func)\
	{r->report.do_##_name = func;}
xwindow_register_event$def(close)
xwindow_register_event$def(expose)
xwindow_register_event$def(key)
xwindow_register_event$def(button)
xwindow_register_event$def(pointer)
xwindow_register_event$def(area)
xwindow_register_event$def(focus)
xwindow_register_event$def(config)
#undef xwindow_register_event$def

void xwindow_register_clear(xwindow_s *restrict r)
{
	if (r->report.data)
		refer_free(r->report.data);
	memset(&r->report, 0, sizeof(r->report));
}
