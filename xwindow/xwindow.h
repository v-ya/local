#ifndef _xwindow_h_
#define _xwindow_h_

#include <refer.h>

typedef struct xwindow_s xwindow_s;
typedef struct xwindow_event_s xwindow_event_s;
typedef struct xwindow_image_s xwindow_image_s;
typedef struct xwindow_cursor_s xwindow_cursor_s;

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

xwindow_s* xwindow_alloc(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t depth);
xwindow_s* xwindow_map(xwindow_s *restrict r);
xwindow_s* xwindow_unmap(xwindow_s *restrict r);
const xwindow_s* xwindow_get_screen_size(const xwindow_s *restrict r, uint32_t *restrict width_pixels, uint32_t *restrict height_pixels, uint32_t *restrict width_mm, uint32_t *restrict height_mm, uint32_t *restrict depth);
const xwindow_s* xwindow_get_geometry(const xwindow_s *restrict r, uint32_t *restrict width, uint32_t *restrict height, int32_t *restrict x, int32_t *restrict y, uint32_t *restrict depth);
xwindow_s* xwindow_set_position(xwindow_s *restrict r, int32_t x, int32_t y);
xwindow_s* xwindow_set_size(xwindow_s *restrict r, uint32_t width, uint32_t height);
xwindow_s* xwindow_set_title(xwindow_s *restrict r, const char *restrict utf8_string);
xwindow_s* xwindow_set_icon(xwindow_s *restrict r, uint32_t width, uint32_t height, const uint32_t *restrict data_bgra);
xwindow_s* xwindow_set_hint_decorations(xwindow_s *restrict r, uint32_t enable);
xwindow_s* xwindow_set_fullscreen(xwindow_s *restrict r, uint32_t enable);

xwindow_event_s* xwindow_event_alloc(xwindow_s *restrict xwindow, const xwindow_event_t *restrict events);
xwindow_event_s* xwindow_event_used(xwindow_event_s *restrict r, const xwindow_event_t *restrict events);
void xwindow_event_clear_register(xwindow_event_s *restrict r);
void xwindow_event_register_close(xwindow_event_s *restrict r, xwindow_event_close_f func, refer_t data);
void xwindow_event_register_expose(xwindow_event_s *restrict r, xwindow_event_expose_f func, refer_t data);
void xwindow_event_register_key(xwindow_event_s *restrict r, xwindow_event_key_f func, refer_t data);
void xwindow_event_register_button(xwindow_event_s *restrict r, xwindow_event_button_f func, refer_t data);
void xwindow_event_register_pointer(xwindow_event_s *restrict r, xwindow_event_pointer_f func, refer_t data);
void xwindow_event_register_area(xwindow_event_s *restrict r, xwindow_event_area_f func, refer_t data);
void xwindow_event_register_focus(xwindow_event_s *restrict r, xwindow_event_focus_f func, refer_t data);
void xwindow_event_register_config(xwindow_event_s *restrict r, xwindow_event_config_f func, refer_t data);
xwindow_event_s* xwindow_event_do(xwindow_event_s *restrict r);
uintptr_t xwindow_event_done(xwindow_event_s *restrict r);

xwindow_image_s* xwindow_image_alloc_memory(xwindow_s *restrict xwindow, uint32_t width_max, uint32_t height_max);
xwindow_image_s* xwindow_image_alloc_shm(xwindow_s *restrict xwindow, uint32_t width_max, uint32_t height_max);
uint32_t* xwindow_image_map(xwindow_image_s *restrict r, uint32_t width, uint32_t height);
xwindow_image_s* xwindow_image_update_full(xwindow_image_s *restrict r, int32_t dx, int32_t dy);
xwindow_image_s* xwindow_image_update_block(xwindow_image_s *restrict r, uint32_t w, uint32_t h, int32_t sx, int32_t sy, int32_t dx, int32_t dy);

xwindow_cursor_s* xwindow_cursor_alloc(xwindow_s *restrict xwindow, uint32_t width, uint32_t height, uint32_t xpos, uint32_t ypos, uint32_t color0, uint32_t color1, uintptr_t bits_size, const void *restrict source_bits, const void *restrict mask_bits);
xwindow_cursor_s* xwindow_cursor_enable(xwindow_cursor_s *restrict cursor);
xwindow_s* xwindow_cursor_disable(xwindow_s *restrict xwindow);

xwindow_s* xwindow_move_pointer(xwindow_s *restrict xwindow, int32_t xpos, int32_t ypos);

#endif
