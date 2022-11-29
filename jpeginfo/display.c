#include "display.h"

static void display_close_func(yaw_s *restrict yaw, xwindow_s *w)
{
	yaw_stop(yaw);
}

static void display_yaw_func(yaw_s *restrict yaw)
{
	xwindow_s *restrict xw;
	xwindow_event_s *restrict e;
	xw = (xwindow_s *) yaw->pri;
	if ((e = xwindow_event_alloc(xw, (const xwindow_event_t []) {xwindow_event_close, xwindow_event_null})))
	{
		xwindow_event_register_close(e, (xwindow_event_close_f) display_close_func, yaw);
		while (yaw->running)
		{
			xwindow_event_done(e);
			yaw_msleep(50);
		}
		refer_free(e);
	}
	xwindow_unmap(xw);
}

static void display_free_func(display_s *restrict r)
{
	if (r->yaw)
	{
		yaw_stop_and_wait(r->yaw);
		refer_free(r->yaw);
	}
	if (r->image) refer_free(r->image);
	if (r->xw) refer_free(r->xw);
}

display_s* display_alloc(uint32_t width, uint32_t height)
{
	display_s *restrict r;
	if ((r = (display_s *) refer_alloz(sizeof(display_s))))
	{
		refer_set_free(r, (refer_free_f) display_free_func);
		if ((r->xw = xwindow_alloc(0, 0, width, height, 24)) && xwindow_map(r->xw) &&
			(r->image = xwindow_image_alloc_shm(r->xw, width, height)) &&
			(r->yaw = yaw_alloc_and_start(display_yaw_func, r->xw, NULL)))
		{
			yaw_msleep(50);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

void display_wait_close(display_s *restrict d)
{
	yaw_wait(d->yaw);
}
