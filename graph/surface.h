#ifndef _graph_surface_h_
#define _graph_surface_h_

#include <refer.h>
#include "type.h"

typedef struct graph_surface_s graph_surface_s;
typedef struct graph_surface_attr_s graph_surface_attr_s;
typedef struct graph_swapchain_param_s graph_swapchain_param_s;
typedef struct graph_swapchain_s graph_swapchain_s;

struct graph_s;
struct graph_device_t;
struct graph_dev_s;
struct graph_semaphore_s;
struct graph_fence_s;

typedef struct graph_surface_geometry_t {
	int32_t x;
	int32_t y;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
} graph_surface_geometry_t;

typedef enum graph_surface_event_e {
	graph_surface_event_null,      // 事件队列结束符
	graph_surface_event_close,     // 关闭事件
	graph_surface_event_expose,    // 绘制事件
	graph_surface_event_key,       // 按键事件
	graph_surface_event_button,    // 鼠标点击事件
	graph_surface_event_pointer,   // 鼠标移动事件
	graph_surface_event_area,      // 鼠标进出窗口事件
	graph_surface_event_focus,     // 窗口焦点事件
	graph_surface_event_resize,    // 窗口大小变更事件
	graph_surface_event$number
} graph_surface_event_t;

typedef struct graph_surface_do_event_state_t {
	int32_t x;
	int32_t y;
	int32_t root_x;
	int32_t root_y;
	uint32_t state;
} graph_surface_do_event_state_t;

typedef void (*graph_surface_do_event_close_f)(refer_t data, graph_surface_s *surface);
typedef void (*graph_surface_do_event_expose_f)(refer_t data, graph_surface_s *surface, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
typedef void (*graph_surface_do_event_key_f)(refer_t data, graph_surface_s *surface, uint32_t key, uint32_t press, graph_surface_do_event_state_t *restrict state);
typedef void (*graph_surface_do_event_button_f)(refer_t data, graph_surface_s *surface, uint32_t button, uint32_t press, graph_surface_do_event_state_t *restrict state);
typedef void (*graph_surface_do_event_pointer_f)(refer_t data, graph_surface_s *surface, graph_surface_do_event_state_t *restrict state);
typedef void (*graph_surface_do_event_area_f)(refer_t data, graph_surface_s *surface, graph_surface_do_event_state_t *restrict state);
typedef void (*graph_surface_do_event_focus_f)(refer_t data, graph_surface_s *surface, uint32_t focus);
typedef void (*graph_surface_do_event_resize_f)(refer_t data, graph_surface_s *surface, uint32_t width, uint32_t height);

graph_surface_s* graph_surface_xcb_create_window(struct graph_s *restrict g, graph_surface_s *restrict parent, int x, int y, unsigned int width, unsigned int height, unsigned int depth);
graph_surface_attr_s* graph_surface_attr_get(const graph_surface_s *restrict surface, const struct graph_device_t *restrict device);

graph_surface_s* graph_surface_do_event(graph_surface_s *restrict surface);
void graph_surface_do_events(graph_surface_s *restrict surface);

void graph_surface_set_event_data(graph_surface_s *restrict surface, refer_t data);
void graph_surface_register_event_close(graph_surface_s *restrict surface, graph_surface_do_event_close_f func);
void graph_surface_register_event_expose(graph_surface_s *restrict surface, graph_surface_do_event_expose_f func);
void graph_surface_register_event_key(graph_surface_s *restrict surface, graph_surface_do_event_key_f func);
void graph_surface_register_event_button(graph_surface_s *restrict surface, graph_surface_do_event_button_f func);
void graph_surface_register_event_pointer(graph_surface_s *restrict surface, graph_surface_do_event_pointer_f func);
void graph_surface_register_event_area(graph_surface_s *restrict surface, graph_surface_do_event_area_f func);
void graph_surface_register_event_focus(graph_surface_s *restrict surface, graph_surface_do_event_focus_f func);
void graph_surface_register_event_resize(graph_surface_s *restrict surface, graph_surface_do_event_resize_f func);

graph_surface_s* graph_surface_set_event(graph_surface_s *restrict surface, const graph_surface_event_t *restrict events);
graph_surface_s* graph_surface_get_geometry(const graph_surface_s *restrict surface, graph_surface_geometry_t *restrict geometry);

graph_swapchain_param_s* graph_swapchain_param_alloc(graph_surface_s *restrict surface, const graph_surface_attr_s *restrict attr, uint32_t queue_sharing_number);
graph_swapchain_s* graph_swapchain_alloc(const graph_swapchain_param_s *restrict param, struct graph_dev_s *dev);
graph_swapchain_s* graph_swapchain_rebulid(graph_swapchain_s *restrict swapchain);
uint32_t graph_swapchain_image_number(const graph_swapchain_s *restrict swapchain);
graph_format_t graph_swapchain_format(const graph_swapchain_s *restrict swapchain);
void graph_swapchain_info(const graph_swapchain_s *restrict swapchain, uint32_t *restrict image_number, graph_format_t *restrict format, uint32_t *restrict width, uint32_t *restrict height);
uint32_t graph_swapchain_acquire(graph_swapchain_s *restrict swapchain, uint64_t timeout, struct graph_semaphore_s *restrict semaphore, struct graph_fence_s *restrict fence);

void graph_surface_attr_dump(const graph_surface_attr_s *restrict attr);

#endif
