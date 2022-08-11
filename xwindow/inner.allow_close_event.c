#include "xwindow.private.h"
#include "xwindow.atom.h"

xcb_connection_t* xwindow_inner_allow_close_event(xcb_connection_t *restrict c, xwindow_atom_s *restrict atom, xcb_window_t window)
{
	xcb_generic_error_t *restrict error;
	xcb_atom_t wm_protocols, wm_delete_window;
	if (xwindow_atom_get(atom, xatom__WM_PROTOCOLS, &wm_protocols, c) &&
		xwindow_atom_get(atom, xatom__WM_DELETE_WINDOW, &wm_delete_window, c))
	{
		if (!(error = xcb_request_check(c, xcb_change_property_checked(
				c, XCB_PROP_MODE_REPLACE, window,
				wm_protocols, XCB_ATOM_ATOM, 32, 1,
				&wm_delete_window))))
			return c;
		free(error);
	}
	return NULL;
}
