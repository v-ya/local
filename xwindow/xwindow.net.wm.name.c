#include "xwindow.private.h"
#include <string.h>

xwindow_s* xwindow_set_title(xwindow_s *restrict r, const char *restrict utf8_string)
{
	xcb_generic_error_t *error;
	if (utf8_string && r->atom.list[xwindow_atom_id___net_wm_name] && r->atom.list[xwindow_atom_id__utf8_string])
	{
		if (!(error = xcb_request_check(r->connection, xcb_change_property_checked(
			r->connection, XCB_PROP_MODE_REPLACE, r->window,
			r->atom.list[xwindow_atom_id___net_wm_name],
			r->atom.list[xwindow_atom_id__utf8_string],
			8, strlen(utf8_string), utf8_string
			))))
		return r;
		free(error);
	}
	return NULL;
}
