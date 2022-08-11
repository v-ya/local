#ifndef _xwindow_private_h_
#define _xwindow_private_h_

#include "xwindow.h"
#include <hashmap.h>
#include <yaw.h>
#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <stdlib.h>

typedef struct xwindow_image_update_t {
	uint32_t w;
	uint32_t h;
	int32_t sx;
	int32_t sy;
	int32_t dx;
	int32_t dy;
} xwindow_image_update_t;

typedef struct xwindow_memory_s {
	void *addr;
	uintptr_t size;
} xwindow_memory_s;

typedef struct xwindow_shm_s {
	void *addr;
	uintptr_t size;
	uint32_t shmid;
	uint32_t attached;
} xwindow_shm_s;

typedef struct xwindow_atom_s {
	yaw_lock_s *read;
	yaw_lock_s *write;
	hashmap_t atom;
} xwindow_atom_s;

struct xwindow_s {
	xwindow_atom_s *atom;
	xcb_connection_t *connection;
	const xcb_screen_t *screen;
	xcb_visualid_t visual;
	xcb_colormap_t colormap;
	xcb_window_t window;
	xcb_gcontext_t gcontext;
	uint32_t depth;
	uint32_t max_request_length;
};

struct xwindow_event_s {
	xwindow_s *xwindow;
	xcb_event_mask_t emask;
	xcb_atom_t wm_delete_window;
	yaw_lock_s *read;
	yaw_lock_s *write;
	// report
	xwindow_event_close_f   func_close;
	xwindow_event_expose_f  func_expose;
	xwindow_event_key_f     func_key;
	xwindow_event_button_f  func_button;
	xwindow_event_pointer_f func_pointer;
	xwindow_event_area_f    func_area;
	xwindow_event_focus_f   func_focus;
	xwindow_event_config_f  func_config;
	refer_t data_close;
	refer_t data_expose;
	refer_t data_key;
	refer_t data_button;
	refer_t data_pointer;
	refer_t data_area;
	refer_t data_focus;
	refer_t data_config;
};

typedef xwindow_image_s* (*xwindow_image_update_f)(xwindow_image_s *restrict image, const xwindow_image_update_t *restrict update);

struct xwindow_image_s {
	xwindow_image_update_f update;
	xwindow_s *xwindow;
	uintptr_t pixels_max;
	uint32_t *pixels;
	uint32_t width;
	uint32_t height;
};

// inner.atom.c

xwindow_atom_s* xwindow_atom_alloc(void);
void xwindow_atom_touch(xwindow_atom_s *restrict r, xcb_connection_t *restrict connection, uintptr_t number, const char *const restrict name_list[]);
xwindow_atom_s* xwindow_atom_get(xwindow_atom_s *restrict r, const char *restrict name, xcb_atom_t *restrict atom, xcb_connection_t *restrict connection);

// inner.atom_touch.c

void xwindow_inner_atom_touch_common(xwindow_atom_s *restrict r, xcb_connection_t *restrict c);

// inner.get_screen.c

const xcb_screen_t* xwindow_inner_get_screen(xcb_connection_t *restrict c, uint32_t depth, xcb_visualid_t *restrict visual);

// inner.allow_close_event.c

xcb_connection_t* xwindow_inner_allow_close_event(xcb_connection_t *restrict c, xwindow_atom_s *restrict atom, xcb_window_t window);

// inner.shm.c

xwindow_memory_s* xwindow_memory_alloc(uintptr_t size);
xwindow_shm_s* xwindow_shm_alloc(uintptr_t size);
void xwindow_shm_remove(xwindow_shm_s *restrict r);

#endif
