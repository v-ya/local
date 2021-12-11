#include "xwindow.private.h"

static void xwindow_free_func(xwindow_s *restrict r)
{
	if (r->connection)
	{
		if (r->shmseg)
			xcb_shm_detach(r->connection, r->shmseg);
		if (r->gcontext)
			xcb_free_gc(r->connection, r->gcontext);
		if (r->window)
			xcb_destroy_window(r->connection, r->window);
		if (r->colormap)
			xcb_free_colormap(r->connection, r->colormap);
		xcb_flush(r->connection);
		xcb_disconnect(r->connection);
	}
	if (r->shm)
		refer_free(r->shm);
	if (r->report.data)
		refer_free(r->report.data);
}

static inline void xwindow_fix_xywh(const xcb_screen_t *restrict screen, int32_t *restrict x, int32_t *restrict y, uint32_t *restrict w, uint32_t *restrict h)
{
	if (!w) *x = (*w = (uint32_t) screen->width_in_pixels / 2) / 2;
	if (!h) *y = (*h = (uint32_t) screen->height_in_pixels / 2) / 2;
}

xwindow_s* xwindow_alloc(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t depth)
{
	xwindow_s *restrict r;
	xcb_generic_error_t *restrict error;
	r = (xwindow_s *) refer_alloz(sizeof(xwindow_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) xwindow_free_func);
		error = NULL;
		r->connection = xcb_connect(NULL, NULL);
		if (!r->connection)
			goto label_fail;
		if (!xwindow_inner_get_atom(r->connection, &r->atom))
			goto label_fail;
		r->screen = xwindow_inner_get_screen(r->connection, depth, &r->visual);
		if (!r->screen)
			goto label_fail;
		r->colormap = xcb_generate_id(r->connection);
		if ((error = xcb_request_check(r->connection, xcb_create_colormap_checked(
				r->connection, XCB_COLORMAP_ALLOC_NONE,
				r->colormap, r->screen->root, r->visual))))
			goto label_fail;
		xwindow_fix_xywh(r->screen, &x, &y, &w, &h);
		r->window = xcb_generate_id(r->connection);
		if ((error = xcb_request_check(r->connection, xcb_create_window_checked(
				r->connection, (uint8_t) depth, r->window, r->screen->root,
				(int16_t) x, (int16_t) y, (uint16_t) w, (uint16_t) h, 0,
				XCB_WINDOW_CLASS_INPUT_OUTPUT, r->visual,
				XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_COLORMAP,
				(uint32_t []) {0, 0, r->colormap}))))
			goto label_fail;
		if (!xwindow_inner_allow_close_event(r->connection, &r->atom, r->window))
			goto label_fail;
		r->gcontext = xcb_generate_id(r->connection);
		if ((error = xcb_request_check(r->connection, xcb_create_gc_checked(
				r->connection, r->gcontext, r->window, 0, NULL))))
			goto label_fail;
		r->depth = depth?depth:r->screen->root_depth;
		r->max_request_length = xcb_get_maximum_request_length(r->connection);
		if (r->max_request_length > 1024) r->max_request_length -= 1024;
		else r->max_request_length = 0;
		if (xcb_flush(r->connection) <= 0)
			goto label_fail;
		return r;
		label_fail:
		if (error) free(error);
		refer_free(r);
	}
	return NULL;
}
