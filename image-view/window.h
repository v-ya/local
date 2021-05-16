#ifndef _image_view_window_h_
#define _image_view_window_h_

#include <refer.h>

typedef struct window_s window_s;

typedef enum window_event_t {
	window_event_null,      // 事件队列结束符
	window_event_close,     // 关闭事件
	window_event_expose,    // 绘制事件
	window_event_key,       // 按键事件
	window_event_button,    // 鼠标点击事件
	window_event_pointer,   // 鼠标移动事件
	window_event_area,      // 鼠标进出窗口事件
	window_event_focus,     // 窗口焦点事件
	window_event_config,    // 窗口配置变更事件
	window_event$number
} window_event_t;

typedef struct window_event_state_t {
	int32_t x;
	int32_t y;
	int32_t root_x;
	int32_t root_y;
	uint32_t state;
} window_event_state_t;

typedef void (*window_event_close_f)(refer_t data, window_s *window);
typedef void (*window_event_expose_f)(refer_t data, window_s *window, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
typedef void (*window_event_key_f)(refer_t data, window_s *window, uint32_t key, uint32_t press, window_event_state_t *restrict state);
typedef void (*window_event_button_f)(refer_t data, window_s *window, uint32_t button, uint32_t press, window_event_state_t *restrict state);
typedef void (*window_event_pointer_f)(refer_t data, window_s *window, window_event_state_t *restrict state);
typedef void (*window_event_area_f)(refer_t data, window_s *window, window_event_state_t *restrict state);
typedef void (*window_event_focus_f)(refer_t data, window_s *window, uint32_t focus);
typedef void (*window_event_config_f)(refer_t data, window_s *window, int32_t x, int32_t y, uint32_t width, uint32_t height);

void window_usleep(uint32_t us);
window_s* window_alloc(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t depth);
window_s* window_enable_shm(window_s *restrict r, uintptr_t shm_size);
window_s* window_disable_shm(window_s *restrict r);
window_s* window_map(window_s *restrict r);
window_s* window_unmap(window_s *restrict r);
window_s* window_update(window_s *restrict r, const uint32_t *restrict data, uint32_t width, uint32_t height, int32_t x, int32_t y);
window_s* window_set_event(window_s *restrict r, const window_event_t *restrict events);
window_s* window_do_event(window_s *restrict r);
uintptr_t window_do_all_events(window_s *restrict r);
void window_register_event_data(window_s *restrict r, refer_t data);
void window_register_event_close(window_s *restrict r, window_event_close_f func);
void window_register_event_expose(window_s *restrict r, window_event_expose_f func);
void window_register_event_key(window_s *restrict r, window_event_key_f func);
void window_register_event_button(window_s *restrict r, window_event_button_f func);
void window_register_event_pointer(window_s *restrict r, window_event_pointer_f func);
void window_register_event_area(window_s *restrict r, window_event_area_f func);
void window_register_event_focus(window_s *restrict r, window_event_focus_f func);
void window_register_event_config(window_s *restrict r, window_event_config_f func);
void window_register_clear(window_s *restrict r);
const window_s* window_get_screen_size(const window_s *restrict r, uint32_t *restrict width_pixels, uint32_t *restrict height_pixels, uint32_t *restrict width_mm, uint32_t *restrict height_mm, uint32_t *restrict depth);
const window_s* window_get_geometry(const window_s *restrict r, uint32_t *restrict width, uint32_t *restrict height, int32_t *restrict x, int32_t *restrict y, uint32_t *restrict depth);
window_s* window_set_position(window_s *restrict r, int32_t x, int32_t y);
window_s* window_set_size(window_s *restrict r, uint32_t width, uint32_t height);
window_s* window_set_hint_decorations(window_s *restrict r, uint32_t enable);
window_s* window_set_fullscreen(window_s *restrict r, uint32_t enable);

#endif
