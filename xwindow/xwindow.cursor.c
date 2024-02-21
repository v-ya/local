#include "xwindow.private.h"

static xcb_connection_t* xwindow_cursor_pixmap_put(xcb_connection_t *restrict c, xcb_pixmap_t pixmap, uint32_t width, uint32_t height, const void *data, uintptr_t size)
{
	xcb_generic_error_t *restrict error;
	xcb_gcontext_t gc;
	gc = xcb_generate_id(c);
	if ((error = xcb_request_check(c, xcb_create_gc_checked(c, gc, pixmap, 0, NULL))))
		goto label_fail;
	if ((error = xcb_request_check(c, xcb_put_image_checked(
		c, XCB_IMAGE_FORMAT_XY_PIXMAP, pixmap, gc,
		(uint16_t) width, (uint16_t) height, 0, 0, 0,
		1, (uint32_t) size, data))))
		goto label_fail;
	xcb_free_gc(c, gc);
	xcb_flush(c);
	return c;
	label_fail:
	free(error);
	xcb_free_gc(c, gc);
	xcb_flush(c);
	return NULL;
}

static void xwindow_cursor_free_func(xwindow_cursor_s *restrict r)
{
	if (r->xwindow)
	{
		if (r->cursor) xcb_free_cursor(r->xwindow->connection, r->cursor);
		if (r->mask) xcb_free_pixmap(r->xwindow->connection, r->mask);
		if (r->source) xcb_free_pixmap(r->xwindow->connection, r->source);
		xcb_flush(r->xwindow->connection);
		refer_free(r->xwindow);
	}
}

xwindow_cursor_s* xwindow_cursor_alloc(xwindow_s *restrict xwindow, uint32_t width, uint32_t height, uint32_t xpos, uint32_t ypos, uint32_t color0, uint32_t color1, uintptr_t bits_size, const void *restrict source_bits, const void *restrict mask_bits)
{
	xwindow_cursor_s *restrict r;
	xcb_generic_error_t *restrict error;
	if (width && height && (r = (xwindow_cursor_s *) refer_alloz(sizeof(xwindow_cursor_s))))
	{
		refer_set_free(r, (refer_free_f) xwindow_cursor_free_func);
		r->xwindow = (xwindow_s *) refer_save(xwindow);
		error = NULL;
		r->source = xcb_generate_id(xwindow->connection);
		if ((error = xcb_request_check(r->xwindow->connection, xcb_create_pixmap_checked(
			r->xwindow->connection, 1, r->source, xwindow->screen->root,
			(uint16_t) width, (uint16_t) height))))
			goto label_fail;
		r->mask = xcb_generate_id(xwindow->connection);
		if ((error = xcb_request_check(r->xwindow->connection, xcb_create_pixmap_checked(
			r->xwindow->connection, 1, r->mask, xwindow->screen->root,
			(uint16_t) width, (uint16_t) height))))
			goto label_fail;
		if (source_bits && bits_size && !xwindow_cursor_pixmap_put(xwindow->connection,
			r->source, width, height, source_bits, bits_size))
			goto label_fail;
		if (mask_bits && bits_size && !xwindow_cursor_pixmap_put(xwindow->connection,
			r->mask, width, height, mask_bits, bits_size))
			goto label_fail;
		r->cursor = xcb_generate_id(xwindow->connection);
		if ((error = xcb_request_check(r->xwindow->connection, xcb_create_cursor_checked(
			r->xwindow->connection, r->cursor, r->source, r->mask,
			(uint16_t) (((color1 & 0x00ff0000) >> 16) << 8),
			(uint16_t) (((color1 & 0x0000ff00) >> 8) << 8),
			(uint16_t) ((color1 & 0x000000ff) << 8),
			(uint16_t) (((color0 & 0x00ff0000) >> 16) << 8),
			(uint16_t) (((color0 & 0x0000ff00) >> 8) << 8),
			(uint16_t) ((color0 & 0x000000ff) << 8),
			(uint16_t) xpos, (uint16_t) ypos))))
			goto label_fail;
		return r;
		label_fail:
		if (error) free(error);
		refer_free(r);
	}
	return NULL;
}

xwindow_cursor_s* xwindow_cursor_enable(xwindow_cursor_s *restrict cursor)
{
	xwindow_s *restrict xwindow;
	xcb_generic_error_t *restrict error;
	xwindow = cursor->xwindow;
	if (!(error = xcb_request_check(xwindow->connection, xcb_change_window_attributes_checked(
		xwindow->connection, xwindow->window, XCB_CW_CURSOR, (const uint32_t []) { cursor->cursor }))))
		return cursor;
	free(error);
	return NULL;
}

xwindow_s* xwindow_cursor_disable(xwindow_s *restrict xwindow)
{
	xcb_generic_error_t *restrict error;
	if (!(error = xcb_request_check(xwindow->connection, xcb_change_window_attributes_checked(
		xwindow->connection, xwindow->window, XCB_CW_CURSOR, (const uint32_t []) { 0 }))))
		return xwindow;
	free(error);
	return NULL;
}
