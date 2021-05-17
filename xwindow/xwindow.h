#ifndef _xwindow_h_
#define _xwindow_h_

#include <refer.h>

typedef struct xwindow_s xwindow_s;

typedef enum xwindow_event_t {
	xwindow_event_null,      // 事件队列结束符
	xwindow_event_close,     // 关闭事件
	xwindow_event_expose,    // 绘制事件
	xwindow_event_key,       // 按键事件
	xwindow_event_button,    // 鼠标点击事件
	xwindow_event_pointer,   // 鼠标移动事件
	xwindow_event_area,      // 鼠标进出窗口事件
	xwindow_event_focus,     // 窗口焦点事件
	xwindow_event_config,    // 窗口配置变更事件
	xwindow_event$number
} xwindow_event_t;

typedef enum xwindow_state_t {
	xwindow_state_shift         = 0x0001,
	xwindow_state_caps_lock     = 0x0002,
	xwindow_state_ctrl          = 0x0004,
	xwindow_state_alt           = 0x0008,
	xwindow_state_num_lock      = 0x0010,
	xwindow_state_mouse_l_press = 0x0100,
	xwindow_state_mouse_m_press = 0x0200,
	xwindow_state_mouse_r_press = 0x0400,
	xwindow_state_mouse_m_up    = 0x0800,
	xwindow_state_mouse_m_down  = 0x1000,
	// mask
	xwindow_state_mask_control  = 0x000d,
	xwindow_state_mask_mouse    = 0x1f00
} xwindow_state_t;

typedef enum xwindow_key_t {
	xwindow_key_esc   = 9,
	xwindow_key_space = 65,
	xwindow_key_f11   = 95
} xwindow_key_t;

typedef enum xwindow_button_t {
	xwindow_button_mouse_l      = 1,
	xwindow_button_mouse_m      = 2,
	xwindow_button_mouse_r      = 3,
	xwindow_button_mouse_m_up   = 4,
	xwindow_button_mouse_m_down = 5
} xwindow_button_t;

typedef struct xwindow_event_state_t {
	int32_t x;
	int32_t y;
	int32_t root_x;
	int32_t root_y;
	xwindow_state_t state;
} xwindow_event_state_t;

typedef void (*xwindow_event_close_f)(refer_t data, xwindow_s *w);
typedef void (*xwindow_event_expose_f)(refer_t data, xwindow_s *w, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
typedef void (*xwindow_event_key_f)(refer_t data, xwindow_s *w, xwindow_key_t key, uint32_t press, xwindow_event_state_t *restrict state);
typedef void (*xwindow_event_button_f)(refer_t data, xwindow_s *w, xwindow_button_t button, uint32_t press, xwindow_event_state_t *restrict state);
typedef void (*xwindow_event_pointer_f)(refer_t data, xwindow_s *w, xwindow_event_state_t *restrict state);
typedef void (*xwindow_event_area_f)(refer_t data, xwindow_s *w, xwindow_event_state_t *restrict state);
typedef void (*xwindow_event_focus_f)(refer_t data, xwindow_s *w, uint32_t focus);
typedef void (*xwindow_event_config_f)(refer_t data, xwindow_s *w, int32_t x, int32_t y, uint32_t width, uint32_t height);

void xwindow_usleep(uint32_t us);
xwindow_s* xwindow_alloc(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t depth);
xwindow_s* xwindow_enable_shm(xwindow_s *restrict r, uintptr_t shm_size);
xwindow_s* xwindow_disable_shm(xwindow_s *restrict r);
xwindow_s* xwindow_map(xwindow_s *restrict r);
xwindow_s* xwindow_unmap(xwindow_s *restrict r);
xwindow_s* xwindow_update(xwindow_s *restrict r, const uint32_t *restrict data, uint32_t width, uint32_t height, int32_t x, int32_t y);
xwindow_s* xwindow_set_event(xwindow_s *restrict r, const xwindow_event_t *restrict events);
xwindow_s* xwindow_do_event(xwindow_s *restrict r);
uintptr_t xwindow_do_all_events(xwindow_s *restrict r);
void xwindow_register_event_data(xwindow_s *restrict r, refer_t data);
void xwindow_register_event_close(xwindow_s *restrict r, xwindow_event_close_f func);
void xwindow_register_event_expose(xwindow_s *restrict r, xwindow_event_expose_f func);
void xwindow_register_event_key(xwindow_s *restrict r, xwindow_event_key_f func);
void xwindow_register_event_button(xwindow_s *restrict r, xwindow_event_button_f func);
void xwindow_register_event_pointer(xwindow_s *restrict r, xwindow_event_pointer_f func);
void xwindow_register_event_area(xwindow_s *restrict r, xwindow_event_area_f func);
void xwindow_register_event_focus(xwindow_s *restrict r, xwindow_event_focus_f func);
void xwindow_register_event_config(xwindow_s *restrict r, xwindow_event_config_f func);
void xwindow_register_clear(xwindow_s *restrict r);
const xwindow_s* xwindow_get_screen_size(const xwindow_s *restrict r, uint32_t *restrict width_pixels, uint32_t *restrict height_pixels, uint32_t *restrict width_mm, uint32_t *restrict height_mm, uint32_t *restrict depth);
const xwindow_s* xwindow_get_geometry(const xwindow_s *restrict r, uint32_t *restrict width, uint32_t *restrict height, int32_t *restrict x, int32_t *restrict y, uint32_t *restrict depth);
xwindow_s* xwindow_set_position(xwindow_s *restrict r, int32_t x, int32_t y);
xwindow_s* xwindow_set_size(xwindow_s *restrict r, uint32_t width, uint32_t height);
xwindow_s* xwindow_set_hint_decorations(xwindow_s *restrict r, uint32_t enable);
xwindow_s* xwindow_set_fullscreen(xwindow_s *restrict r, uint32_t enable);

#endif
