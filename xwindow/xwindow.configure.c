#include "xwindow.private.h"

const xwindow_s* xwindow_get_geometry(const xwindow_s *restrict r, uint32_t *restrict width, uint32_t *restrict height, int32_t *restrict x, int32_t *restrict y, uint32_t *restrict depth)
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

xwindow_s* xwindow_set_position(xwindow_s *restrict r, int32_t x, int32_t y)
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

xwindow_s* xwindow_set_size(xwindow_s *restrict r, uint32_t width, uint32_t height)
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
