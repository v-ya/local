#include "xwindow.private.h"
#include <memory.h>

xwindow_s* xwindow_update(xwindow_s *restrict r, const uint32_t *restrict data, uint32_t width, uint32_t height, int32_t x, int32_t y)
{
	xcb_generic_error_t *restrict error;
	uint32_t linesize, nl;
	uintptr_t shm_nl;
	if (r->depth != 24 && r->depth != 32)
		goto label_fail;
	linesize = width << 2;
	error = NULL;
	if (r->shm)
	{
		shm_nl = r->shm->size / linesize;
		if (!shm_nl) goto label_fail;
		if ((uintptr_t) (nl = (uint32_t) shm_nl) != shm_nl)
			nl = ~(uint32_t) 0;
		do {
			if (nl > height) nl = height;
			memcpy(r->shm->addr, data, linesize * nl);
			error = xcb_request_check(r->connection, xcb_shm_put_image_checked(
				r->connection, r->window, r->gcontext,
				(uint16_t) width, (uint16_t) nl, 0, 0,
				(uint16_t) width, (uint16_t) nl, (int16_t) x, (int16_t) y,
				r->depth, XCB_IMAGE_FORMAT_Z_PIXMAP, 0, r->shmseg, 0));
			data += width * nl;
			y += nl;
			height -= nl;
		} while (!error && height);
	}
	else
	{
		nl = r->max_request_length / linesize;
		if (!nl) goto label_fail;
		do {
			if (nl > height) nl = height;
			error = xcb_request_check(r->connection, xcb_put_image_checked(
				r->connection, XCB_IMAGE_FORMAT_Z_PIXMAP, r->window, r->gcontext,
				(uint16_t) width, (uint16_t) nl, (int16_t) x, (int16_t) y,
				0, r->depth, linesize * nl, (const uint8_t *) data));
			data += width * nl;
			y += nl;
			height -= nl;
		} while (!error && height);
	}
	if (!error) return r;
	free(error);
	label_fail:
	return NULL;
}
