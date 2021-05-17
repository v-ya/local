#include "xwindow.private.h"

const xwindow_s* xwindow_get_screen_size(const xwindow_s *restrict r, uint32_t *restrict width_pixels, uint32_t *restrict height_pixels, uint32_t *restrict width_mm, uint32_t *restrict height_mm, uint32_t *restrict depth)
{
	const xcb_screen_t *restrict screen;
	if ((screen = r->screen))
	{
		if (width_pixels) *width_pixels = screen->width_in_pixels;
		if (height_pixels) *height_pixels = screen->height_in_pixels;
		if (width_mm) *width_mm = screen->width_in_millimeters;
		if (height_mm) *height_mm = screen->height_in_millimeters;
		if (depth) *depth = screen->root_depth;
		return r;
	}
	return NULL;
}
