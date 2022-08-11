#include "xwindow.private.h"
#include <memory.h>

typedef struct xwindow_image_memory_s {
	xwindow_image_s image;
	xwindow_memory_s *memory;
} xwindow_image_memory_s;

typedef struct xwindow_image_shm_s {
	xwindow_image_s image;
	xwindow_shm_s *shm;
	xcb_shm_seg_t seg;
} xwindow_image_shm_s;

static xwindow_image_s* xwindow_image_memory_update_func(xwindow_image_memory_s *restrict image, const xwindow_image_update_t *restrict update)
{
	xwindow_s *restrict xwindow;
	xcb_connection_t *restrict connection;
	xcb_generic_error_t *restrict error;
	const uint8_t *restrict data;
	uint32_t width, height, linesize, nl, nb;
	int32_t x, y;
	if (update->w == image->image.width && update->h == image->image.height && update->w && update->h && !update->sx && !update->sy)
	{
		xwindow = image->image.xwindow;
		connection = xwindow->connection;
		data = (const uint8_t *) image->image.pixels;
		width = image->image.width;
		height = image->image.height;
		x = update->dx;
		y = update->dy;
		linesize = width << 2;
		nl = xwindow->max_request_length / linesize;
		if (nl)
		{
			do {
				if (nl > height) nl = height;
				nb = linesize * nl;
				error = xcb_request_check(connection, xcb_put_image_checked(
					connection, XCB_IMAGE_FORMAT_Z_PIXMAP, xwindow->window, xwindow->gcontext,
					(uint16_t) width, (uint16_t) nl, (int16_t) x, (int16_t) y,
					0, xwindow->depth, nb, data));
				data += nb;
				y += nl;
				height -= nl;
			} while (!error && height);
			if (!error)
				return &image->image;
			free(error);
		}
	}
	return NULL;
}

static xwindow_image_s* xwindow_image_shm_update_func(xwindow_image_shm_s *restrict image, const xwindow_image_update_t *restrict update)
{
	xwindow_s *restrict xwindow;
	xcb_connection_t *restrict connection;
	xcb_generic_error_t *restrict error;
	if (update->w && update->h)
	{
		xwindow = image->image.xwindow;
		connection = xwindow->connection;
		error = xcb_request_check(connection, xcb_shm_put_image_checked(
				connection, xwindow->window, xwindow->gcontext,
				(uint16_t) image->image.width, (uint16_t) image->image.height,
				(int16_t) update->sx, (int16_t) update->sy,
				(uint16_t) update->w, (uint16_t) update->h,
				(int16_t) update->dx, (int16_t) update->dy,
				xwindow->depth, XCB_IMAGE_FORMAT_Z_PIXMAP, 0, image->seg, 0));
		if (!error)
			return &image->image;
		free(error);
	}
	return NULL;
}

static void xwindow_image_memory_free_func(xwindow_image_memory_s *restrict r)
{
	if (r->memory) refer_free(r->memory);
	if (r->image.xwindow) refer_free(r->image.xwindow);
}

static void xwindow_image_shm_free_func(xwindow_image_shm_s *restrict r)
{
	if (~r->seg) xcb_shm_detach(r->image.xwindow->connection, r->seg);
	if (r->shm) refer_free(r->shm);
	if (r->image.xwindow) refer_free(r->image.xwindow);
}

static xwindow_image_s* xwindow_image_initial(xwindow_image_s *restrict r, xwindow_image_update_f update, xwindow_s *restrict xwindow, void *data, uintptr_t size)
{
	r->update = update;
	r->xwindow = (xwindow_s *) refer_save(xwindow);
	r->pixels_max = size >> 2;
	r->pixels = (uint32_t *) data;
	return r;
}

xwindow_image_s* xwindow_image_alloc_memory(xwindow_s *restrict xwindow, uint32_t width_max, uint32_t height_max)
{
	xwindow_image_memory_s *restrict r;
	r = (xwindow_image_memory_s *) refer_alloz(sizeof(xwindow_image_memory_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) xwindow_image_memory_free_func);
		if ((r->memory = xwindow_memory_alloc((uintptr_t) width_max * height_max * 4)))
			return xwindow_image_initial(&r->image,
				(xwindow_image_update_f) xwindow_image_memory_update_func,
				xwindow, r->memory->addr, r->memory->size);
		refer_free(r);
	}
	return NULL;
}

xwindow_image_s* xwindow_image_alloc_shm(xwindow_s *restrict xwindow, uint32_t width_max, uint32_t height_max)
{
	xwindow_image_shm_s *restrict r;
	xcb_generic_error_t *restrict error;
	xcb_connection_t *restrict connection;
	xcb_shm_seg_t shmseg;
	r = (xwindow_image_shm_s *) refer_alloz(sizeof(xwindow_image_shm_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) xwindow_image_shm_free_func);
		r->seg = ~0;
		if ((r->shm = xwindow_shm_alloc((uintptr_t) width_max * height_max * 4)))
		{
			connection = xwindow->connection;
			shmseg = xcb_generate_id(connection);
			if (!(error = xcb_request_check(connection, xcb_shm_attach_checked(
					connection, shmseg, r->shm->shmid, 1))))
			{
				r->seg = shmseg;
				return xwindow_image_initial(&r->image,
					(xwindow_image_update_f) xwindow_image_shm_update_func,
					xwindow, r->shm->addr, r->shm->size);
			}
			free(error);
		}
		refer_free(r);
	}
	return NULL;
}

uint32_t* xwindow_image_map(xwindow_image_s *restrict r, uint32_t width, uint32_t height)
{
	uintptr_t pixels;
	if ((pixels = (uintptr_t) width * height) <= r->pixels_max)
	{
		r->width = width;
		r->height = height;
		return r->pixels;
	}
	return NULL;
}

xwindow_image_s* xwindow_image_update_full(xwindow_image_s *restrict r, int32_t dx, int32_t dy)
{
	xwindow_image_update_t update;
	update.w = r->width;
	update.h = r->height;
	update.sx = 0;
	update.sy = 0;
	update.dx = dx;
	update.dy = dy;
	return r->update(r, &update);
}

xwindow_image_s* xwindow_image_update_block(xwindow_image_s *restrict r, uint32_t w, uint32_t h, int32_t sx, int32_t sy, int32_t dx, int32_t dy)
{
	xwindow_image_update_t update;
	update.w = w;
	update.h = h;
	update.sx = sx;
	update.sy = sy;
	update.dx = dx;
	update.dy = dy;
	return r->update(r, &update);
}
