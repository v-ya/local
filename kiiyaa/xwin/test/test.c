#include <xwindow.h>

typedef struct data_s {
	uint32_t width;
	uint32_t height;
	uint32_t new_width;
	uint32_t new_height;
	xwindow_image_s *image;
	uint32_t *pixels;
	uintptr_t size_update;
	uintptr_t data_update;
	uintptr_t draw_enable;
	uint32_t color_last;
	uint32_t color_shift;
} data_s;

static void data_free_func(data_s *restrict r)
{
	if (r->image)
		refer_free(r->image);
}

data_s* data_alloc(xwindow_s *restrict xw)
{
	data_s *restrict r;
	uint32_t width, height;
	if (xwindow_get_screen_size(xw, &width, &height, NULL, NULL, NULL))
	{
		if ((r = (data_s *) refer_alloz(sizeof(data_s))))
		{
			refer_set_free(r, (refer_free_f) data_free_func);
			if ((r->image = xwindow_image_alloc_shm(xw, width, height)))
				return r;
			refer_free(r);
		}
	}
	return NULL;
}

static void data_fill(data_s *restrict data, uintptr_t start, uintptr_t stop, uint32_t color)
{
	uint32_t *restrict d;
	if ((d = data->pixels))
	{
		while (start < stop)
		{
			d[start] = color;
			++start;
		}
	}
}

xwindow_s* events_done(xwindow_s *restrict xw, data_s *restrict data)
{
	if (data->size_update)
	{
		if ((data->pixels = xwindow_image_map(data->image, data->new_width, data->new_height)))
		{
			uintptr_t start, stop;
			start = (uintptr_t) data->width * data->height;
			stop = (uintptr_t) data->new_width * data->new_height;
			if (stop > start)
				data_fill(data, start, stop, 0xff000000);
			data->width = data->new_width;
			data->height = data->new_height;
		}
		data->size_update = 0;
	}
	if (data->data_update && data->width && data->height)
	{
		xwindow_image_update_full(data->image, 0, 0);
		data->data_update = 0;
	}
	return xw;
}

void event_close(data_s *restrict data, xwindow_s *w)
{
	xwindow_unmap(w);
}

void event_expose(data_s *restrict data, xwindow_s *w, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	data->data_update = 1;
}

void event_key(data_s *restrict data, xwindow_s *w, xwindow_key_t key, uint32_t press, xwindow_event_state_t *restrict state)
{
	if (key == xwindow_key_space && !press)
	{
		data_fill(data, 0, (uintptr_t) data->width * data->height, 0xff000000);
		data->data_update = 1;
	}
}

void event_button(data_s *restrict data, xwindow_s *w, xwindow_button_t button, uint32_t press, xwindow_event_state_t *restrict state)
{
	data->draw_enable = !!press;
}

void event_pointer(data_s *restrict data, xwindow_s *w, xwindow_event_state_t *restrict state)
{
	if (data->draw_enable && state->x >= 0 && state->y >= 0 &&
		(uint32_t) state->x < data->width && (uint32_t) state->y < data->height)
	{
		data->color_last = (((data->color_last << data->color_shift) |
					(data->color_last >> (32 - data->color_shift)))
					^ (state->x * state->y)) | 0xff000000;
		data->color_shift = (data->color_shift + 1) & 31;
		if (data->pixels)
			data->pixels[(uintptr_t) state->y * data->width + (uintptr_t) state->x] = data->color_last;
		data->data_update = 1;
	}
}

void event_config(data_s *restrict data, xwindow_s *w, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
	if (data->width != width || data->height != height)
	{
		data->new_width = width;
		data->new_height = height;
		data->size_update = 1;
	}
}
