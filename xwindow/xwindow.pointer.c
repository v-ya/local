#include "xwindow.private.h"

xwindow_s* xwindow_move_pointer(xwindow_s *restrict xwindow, int32_t xpos, int32_t ypos)
{
	xcb_generic_error_t *restrict error;
	if (!(error = xcb_request_check(xwindow->connection, xcb_warp_pointer_checked(
		xwindow->connection, 0, xwindow->window, 0, 0, 0, 0, (int16_t) xpos, (int16_t) ypos))))
		return xwindow;
	free(error);
	return NULL;
}
