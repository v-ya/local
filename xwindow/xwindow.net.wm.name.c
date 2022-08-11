#include "xwindow.private.h"
#include "xwindow.atom.h"
#include <string.h>

xwindow_s* xwindow_set_title(xwindow_s *restrict r, const char *restrict utf8_string)
{
	xcb_generic_error_t *error;
	xcb_atom_t atom_net_wm_name, atom_utf8_string;
	if (utf8_string &&
		xwindow_atom_get(r->atom, xatom___NET_WM_NAME, &atom_net_wm_name, r->connection) &&
		xwindow_atom_get(r->atom, xatom__UTF8_STRING, &atom_utf8_string, r->connection))
	{
		if (!(error = xcb_request_check(r->connection, xcb_change_property_checked(
			r->connection, XCB_PROP_MODE_REPLACE, r->window,
			atom_net_wm_name, atom_utf8_string,
			8, strlen(utf8_string), utf8_string
			))))
		return r;
		free(error);
	}
	return NULL;
}
