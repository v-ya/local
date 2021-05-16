#include "inst.h"
#include "window.h"
#include "image_load.h"
#include "image_resample.h"
#include <math.h>

#define  inst_zoom_in_2   0.7071067811865475f
#define  inst_zoom_in_4   0.8408964152537145f
#define  inst_zoom_in_8   0.9170040432046712f
#define  inst_zoom_out_2  1.414213562373095f
#define  inst_zoom_out_4  1.189207115002721f
#define  inst_zoom_out_8  1.090507732665257f

typedef enum inst_task_t {
	inst_task_tran = 0x01, // 移动图片
	inst_task_rota = 0x02, // 旋转图片
	inst_task_move = 0x04, // 移动窗口
	inst_task_size = 0x08, // 缩放窗口
	inst_task_scal = 0x10, // 缩放窗口(保持宽高比例)
} inst_task_t;

typedef enum inst_state_t {
	inst_state_shift         = 0x0001,
	inst_state_caps_lock     = 0x0002,
	inst_state_ctrl          = 0x0004,
	inst_state_alt           = 0x0008,
	inst_state_num_lock      = 0x0010,
	inst_state_mouse_l_press = 0x0100,
	inst_state_mouse_m_press = 0x0200,
	inst_state_mouse_r_press = 0x0400,
	inst_state_mouse_m_up    = 0x0800,
	inst_state_mouse_m_down  = 0x1000,
	// mask
	inst_state_mask_control  = 0x000d,
	inst_state_mask_mouse    = 0x1f00
} inst_state_t;

struct inst_s {
	const image_bgra_s *image;
	image_resample_s *resample;
	window_s *window;
	volatile uint32_t is_close;
	volatile uint32_t update;
	inst_task_t task;
	inst_state_t state;
	uint32_t width;
	uint32_t height;
	int32_t last_x;
	int32_t last_y;
	uint32_t size_w;
	uint32_t size_h;
	uint32_t scal_w;
	uint32_t scal_h;
	uint32_t hint_decorations;
	uint32_t fullscreen;
};

static void inst_event_close_func(inst_s *restrict r, window_s *window)
{
	r->is_close = 1;
}

static void inst_event_expose_func(inst_s *restrict r, window_s *window, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	r->update = 1;
}

static void inst_event_config_func(inst_s *restrict r, window_s *window, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
	if ((r->width != width || r->height != height))
	{
		image_resample_m_tran(r->resample, (float) (int32_t) (r->width - width) * 0.5f, (float) (int32_t) (r->height - height) * 0.5f);
		r->width = width;
		r->height = height;
		r->update = 1;
	}
}

static void inst_event_key_func(inst_s *restrict r, window_s *window, uint32_t key, uint32_t press, window_event_state_t *restrict state)
{
	if (press)
	{
		if (key == 9)
		{
			// <esc>
			r->is_close = 1;
		}
		else if (key == 65)
		{
			// <space>
			if (window_set_hint_decorations(window, !r->hint_decorations))
				r->hint_decorations = !r->hint_decorations;
		}
		else if (key == 95)
		{
			// <F11>
			if (window_set_fullscreen(window, !r->fullscreen))
				r->fullscreen = !r->fullscreen;
		}
	}
}

static void inst_event_button_func(inst_s *restrict r, window_s *window, uint32_t button, uint32_t press, window_event_state_t *restrict state)
{
	inst_state_t task_state; 
	r->last_x = state->x;
	r->last_y = state->y;
	if (press && !r->task)
	{
		task_state = (1u << (button + 7)) | (state->state & inst_state_mask_control);
		switch ((uint32_t) task_state)
		{
			case inst_state_mouse_l_press:
				r->task = inst_task_tran;
				break;
			case inst_state_mouse_r_press:
				r->task = inst_task_rota;
				break;
			case inst_state_mouse_l_press | inst_state_ctrl:
				r->task = inst_task_move;
				break;
			case inst_state_mouse_r_press | inst_state_ctrl:
				r->task = inst_task_size;
				r->size_w = r->width;
				r->size_h = r->height;
				break;
			case inst_state_mouse_r_press | inst_state_shift:
				r->task = inst_task_scal;
				r->scal_w = r->size_w = r->width;
				r->scal_h = r->size_h = r->height;
				break;
			case inst_state_mouse_m_press:
				image_resample_m_reset(r->resample);
				r->update = 1;
				break;
			case inst_state_mouse_m_press | inst_state_ctrl:
				window_set_size(r->window, r->image->width, r->image->height);
				r->update = 1;
				break;
			case inst_state_mouse_m_up:
				image_resample_m_scale(r->resample, (float) state->x, (float) state->y, inst_zoom_in_2);
				r->update = 1;
				break;
			case inst_state_mouse_m_down:
				image_resample_m_scale(r->resample, (float) state->x, (float) state->y, inst_zoom_out_2);
				r->update = 1;
				break;
			case inst_state_mouse_m_up | inst_state_ctrl:
				image_resample_m_scale(r->resample, (float) state->x, (float) state->y, inst_zoom_in_4);
				r->update = 1;
				break;
			case inst_state_mouse_m_down | inst_state_ctrl:
				image_resample_m_scale(r->resample, (float) state->x, (float) state->y, inst_zoom_out_4);
				r->update = 1;
				break;
			case inst_state_mouse_m_up | inst_state_shift:
				image_resample_m_scale(r->resample, (float) state->x, (float) state->y, inst_zoom_in_8);
				r->update = 1;
				break;
			case inst_state_mouse_m_down | inst_state_shift:
				image_resample_m_scale(r->resample, (float) state->x, (float) state->y, inst_zoom_out_8);
				r->update = 1;
				break;
			default:
				goto label_fail;
		}
		r->state = task_state;
	}
	else
	{
		label_fail:
		r->task = 0;
	}
}

static void inst_mouse_pointer_rota(inst_s *restrict r, int32_t x, int32_t y)
{
	float x0, y0, x1, y1, x2, y2;
	x0 = r->width * 0.5f;
	y0 = r->height * 0.5f;
	x1 = r->last_x - x0;
	y1 = r->last_y - y0;
	x2 = x - x0;
	y2 = y - y0;
	if ((x1 * x1 + y1 * y1) > 1 && (x2 * x2 + y2 * y2) > 1)
		image_resample_m_rota(r->resample, x0, y0, atan2f(y1, x1) - atan2f(y2, x2));
}

static void inst_mouse_pointer_size(inst_s *restrict r, int32_t x, int32_t y)
{
	x -= r->last_x;
	y -= r->last_y;
	x += r->size_w;
	y += r->size_h;
	if (x <= 0) x = 1;
	if (y <= 0) y = 1;
	window_set_size(r->window, r->size_w = (uint32_t) x, r->size_h = (uint32_t) y);
}

static void inst_mouse_pointer_scal(inst_s *restrict r, int32_t x, int32_t y)
{
	double k;
	x -= r->last_x;
	y -= r->last_y;
	x += r->size_w;
	y += r->size_h;
	if (x <= 0) x = 1;
	if (y <= 0) y = 1;
	k = sqrt((double) x * y / ((uint64_t) r->scal_w * r->scal_h));
	r->size_w = (uint32_t) (r->scal_w * k + 0.5);
	r->size_h = (uint32_t) (r->scal_h * k + 0.5);
	window_set_size(r->window, r->size_w, r->size_h);
}

static void inst_event_pointer_func(inst_s *restrict r, window_s *window, window_event_state_t *restrict state)
{
	if (r->task)
	{
		if ((state->state & (inst_state_mask_control | inst_state_mask_mouse)) == r->state)
		{
			switch (r->task)
			{
				case inst_task_tran:
					image_resample_m_tran(r->resample, (float) (r->last_x - state->x), (float) (r->last_y - state->y));
					r->update = 1;
					break;
				case inst_task_rota:
					inst_mouse_pointer_rota(r, state->x, state->y);
					r->update = 1;
					break;
				case inst_task_move:
					window_set_position(r->window, state->root_x - r->last_x, state->root_y - r->last_y);
					goto label_skip_update_position;
				case inst_task_size:
					inst_mouse_pointer_size(r, state->x, state->y);
					break;
				case inst_task_scal:
					inst_mouse_pointer_scal(r, state->x, state->y);
					break;
			}
		}
		r->last_x = state->x;
		r->last_y = state->y;
	}
	label_skip_update_position:
	return ;
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

inst_s* inst_alloc(const char *restrict path, uint32_t multicalc, uint32_t bgcolor)
{
	inst_s *restrict r;
	r = (inst_s *) refer_alloz(sizeof(inst_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) inst_free_func);
		r->image = image_bgra_alloc(path);
		if (r->image)
		{
			if ((r->resample = image_resample_alloc(multicalc, bgcolor)) &&
				image_resample_set_src(r->resample, r->image->data, r->image->width, r->image->height))
			{
				r->window = window_alloc(0, 0, r->width = r->image->width, r->height = r->image->height, 24);
				if (r->window)
				{
					r->hint_decorations = 1;
					window_register_event_data(r->window, r);
					window_register_event_close(r->window, (window_event_close_f) inst_event_close_func);
					window_register_event_expose(r->window, (window_event_expose_f) inst_event_expose_func);
					window_register_event_key(r->window, (window_event_key_f) inst_event_key_func);
					window_register_event_button(r->window, (window_event_button_f) inst_event_button_func);
					window_register_event_pointer(r->window, (window_event_pointer_f) inst_event_pointer_func);
					window_register_event_config(r->window, (window_event_config_f) inst_event_config_func);
					return r;
				}
			}
		}
		refer_free(r);
	}
	return NULL;
}

inst_s* inst_enable_shm(inst_s *restrict r, uintptr_t shm_size)
{
	if (!shm_size)
	{
		uint32_t w, h;
		if (!window_get_screen_size(r->window, &w, &h, NULL, NULL, NULL))
			goto label_fail;
		shm_size = sizeof(uint32_t) * w * h;
	}
	window_disable_shm(r->window);
	if (window_enable_shm(r->window, shm_size))
		return r;
	label_fail:
	return NULL;
}

inst_s* inst_begin(inst_s *restrict r)
{
	if (window_set_event(r->window, (const window_event_t []) {
			window_event_close,
			window_event_expose,
			window_event_key,
			window_event_button,
			window_event_pointer,
			window_event_config,
			window_event_null
			}) && window_map(r->window))
		return r;
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
