#include "xwindow.private.h"
#include <memory.h>

xwindow_s* xwindow_set_icon(xwindow_s *restrict r, uint32_t width, uint32_t height, const uint32_t *restrict data_bgra)
{
	uint32_t *restrict data;
	xcb_generic_error_t *error;
	uintptr_t length;
	if (width && height && width <= 256 && height <= 256 && r->atom.list[xwindow_atom_id___net_wm_icon] &&
		(data = (uint32_t *) malloc(((length = width * height) + 2) * sizeof(uint32_t))))
	{
		data[0] = width;
		data[1] = height;
		memcpy(data + 2, data_bgra, length * sizeof(uint32_t));
		error = xcb_request_check(r->connection, xcb_change_property_checked(
			r->connection, XCB_PROP_MODE_REPLACE, r->window,
			r->atom.list[xwindow_atom_id___net_wm_icon],
			XCB_ATOM_CARDINAL,
			32, (uint32_t) (length + 2), data
			));
		free(data);
		if (!error)
			return r;
		free(error);
	}
	return NULL;
}
