#define _DEFAULT_SOURCE
#include "inst.h"
#include <math.h>

static void inst_event_close_func(inst_s *restrict r, window_s *window)
{
	r->is_close = 1;
}

static void inst_event_expose_func(inst_s *restrict r, window_s *window, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	image_resample_m_tran(r->resample, (float) (int32_t) (r->width - width) * 0.5f, (float) (int32_t) (r->height - height) * 0.5f);
	r->width = width;
	r->height = height;
	r->update = 1;
}

static void inst_event_button_func(inst_s *restrict r, window_s *window, uint32_t button, uint32_t press, window_event_state_t *restrict state)
{
	r->last_x = state->x;
	r->last_y = state->y;
	// L button
	if (button == 1) r->b_tran = !!press;
	// R button
	else if (button == 3) r->b_rota = !!press;
	// M button
	else if (button == 2 && press)
	{
		image_resample_m_reset(r->resample);
		r->update = 1;
	}
	// M button up
	else if (button == 4)
	{
		image_resample_m_scale(r->resample, (float) state->x, (float) state->y, M_SQRT1_2);
		r->update = 1;
	}
	// M button down
	else if (button == 5)
	{
		image_resample_m_scale(r->resample, (float) state->x, (float) state->y, M_SQRT2);
		r->update = 1;
	}
}

static void inst_event_pointer_func(inst_s *restrict r, window_s *window, window_event_state_t *restrict state)
{
	switch (state->state & 0x0f00)
	{
		case 0x100:
			// L button
			if (r->b_tran)
			{
				image_resample_m_tran(r->resample, (float) (r->last_x - state->x), (float) (r->last_y - state->y));
				r->update = 1;
			}
			break;
		case 0x400:
			// R button
			if (r->b_rota)
			{
				uint32_t x0, y0;
				int32_t x1, y1, x2, y2;
				x0 = r->width >> 1;
				y0 = r->height >> 1;
				x1 = r->last_x - x0;
				y1 = r->last_y - y0;
				x2 = state->x - x0;
				y2 = state->y - y0;
				if ((x1 || x2) && (x2 || y2))
				{
					image_resample_m_rota(r->resample, (float) x0, (float) y0, atan2f(y1, x1) - atan2f(y2, x2));
					r->update = 1;
				}
			}
			break;
	}
	r->last_x = state->x;
	r->last_y = state->y;
}

static void inst_free_func(inst_s *restrict r)
{
	if (r->window)
	{
		window_unmap(r->window);
		refer_free(r->window);
	}
	if (r->resample)
		refer_free(r->resample);
	if (r->image)
		refer_free(r->image);
}

inst_s* inst_alloc(const char *restrict path)
{
	inst_s *restrict r;
	r = (inst_s *) refer_alloz(sizeof(inst_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) inst_free_func);
		r->image = image_bgra_alloc(path);
		if (r->image)
		{
			if ((r->resample = image_resample_alloc()) &&
				image_resample_set_src(r->resample, r->image->data, r->image->width, r->image->height))
			{
				r->window = window_alloc(0, 0, r->width = r->image->width, r->height = r->image->height, 24);
				if (r->window)
				{
					window_register_event_data(r->window, r);
					window_register_event_close(r->window, (window_event_close_f) inst_event_close_func);
					window_register_event_expose(r->window, (window_event_expose_f) inst_event_expose_func);
					window_register_event_button(r->window, (window_event_button_f) inst_event_button_func);
					window_register_event_pointer(r->window, (window_event_pointer_f) inst_event_pointer_func);
					if (window_set_event(r->window, (const window_event_t []) {
						window_event_close,
						window_event_expose,
						window_event_button,
						window_event_pointer,
						window_event_null
						}) && window_map(r->window))
						return r;
					window_register_clear(r->window);
				}
			}
		}
		refer_free(r);
	}
	return NULL;
}

void inst_free(inst_s *restrict r)
{
	if (r->window)
		window_register_clear(r->window);
	refer_free(r);
}

void inst_wait(inst_s *restrict r)
{
	while (!r->is_close)
	{
		if (!r->update)
			window_usleep(5000);
		else
		{
			if (image_resample_set_dst(r->resample, r->width, r->height))
			{
				if (image_resample_get_dst(r->resample))
				{
					window_update(r->window, r->resample->dst, r->width, r->height, 0, 0);
				}
			}
			r->update = 0;
		}
		window_do_all_events(r->window);
	}
}
