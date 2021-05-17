#include "xwindow.private.h"

xwindow_s* xwindow_map(xwindow_s *restrict r)
{
	xcb_generic_error_t *restrict error;
	if (!(error = xcb_request_check(r->connection, xcb_map_window_checked(r->connection, r->window))))
		return r;
	free(error);
	return NULL;
}

xwindow_s* xwindow_unmap(xwindow_s *restrict r)
{
	xcb_generic_error_t *restrict error;
	if (!(error = xcb_request_check(r->connection, xcb_unmap_window_checked(r->connection, r->window))))
		return r;
	free(error);
	return NULL;
}
