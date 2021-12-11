#include "xwindow.private.h"

xcb_connection_t* xwindow_inner_allow_close_event(xcb_connection_t *restrict c, const xwindow_atom_t *restrict atom, xcb_window_t window)
{
	xcb_generic_error_t *restrict error;
	if (atom->list[xwindow_atom_id__wm_protocols] && atom->list[xwindow_atom_id__wm_delete_window])
	{
		if (!(error = xcb_request_check(c, xcb_change_property_checked(
				c, XCB_PROP_MODE_REPLACE, window,
				atom->list[xwindow_atom_id__wm_protocols], XCB_ATOM_ATOM, 32, 1,
				&atom->list[xwindow_atom_id__wm_delete_window]))))
			return c;
		free(error);
	}
	return NULL;
}
