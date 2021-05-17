#include "xwindow.private.h"

xcb_connection_t* xwindow_inner_allow_close_event(xcb_connection_t *restrict c, xcb_window_t window, xcb_atom_t *restrict atom_close)
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
