#ifndef _xwindow_private_h_
#define _xwindow_private_h_

#include "xwindow.h"
#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <stdlib.h>

typedef struct xwindow_event_report_t {
	refer_t data;
	xwindow_event_close_f   do_close;
	xwindow_event_expose_f  do_expose;
	xwindow_event_key_f     do_key;
	xwindow_event_button_f  do_button;
	xwindow_event_pointer_f do_pointer;
	xwindow_event_area_f    do_area;
	xwindow_event_focus_f   do_focus;
	xwindow_event_config_f  do_config;
} xwindow_event_report_t;

typedef struct xwindow_shm_s {
	void *addr;
	uintptr_t size;
	uint32_t shmid;
	uint32_t remove;
} xwindow_shm_s;

typedef enum xwindow_atom_id_t {
	xwindow_atom_id__utf8_string,
	xwindow_atom_id__wm_protocols,
	xwindow_atom_id__wm_delete_window,
	xwindow_atom_id___motif_wm_hints,
	xwindow_atom_id___net_wm_icon,
	xwindow_atom_id___net_wm_name,
	xwindow_atom_id___net_wm_state,
	xwindow_atom_id___net_wm_state_fullscreen,
	xwindow_atom_id_number
} xwindow_atom_id_t;

typedef struct xwindow_atom_t {
	xcb_atom_t list[xwindow_atom_id_number];
} xwindow_atom_t;

struct xwindow_s {
	xcb_connection_t *connection;
	const xcb_screen_t *screen;
	xwindow_shm_s *shm;
	xcb_visualid_t visual;
	xcb_colormap_t colormap;
	xcb_window_t window;
	xcb_gcontext_t gcontext;
	xcb_shm_seg_t shmseg;
	xwindow_event_report_t report;
	uint32_t depth;
	uint32_t max_request_length;
	xwindow_atom_t atom;
};

// inner

xcb_connection_t* xwindow_inner_get_atom(xcb_connection_t *restrict c, xwindow_atom_t *restrict atom);
const xcb_screen_t* xwindow_inner_get_screen(xcb_connection_t *restrict c, uint32_t depth, xcb_visualid_t *restrict visual);
xcb_connection_t* xwindow_inner_allow_close_event(xcb_connection_t *restrict c, const xwindow_atom_t *restrict atom, xcb_window_t window);

xwindow_shm_s* xwindow_shm_alloc(uintptr_t size);

#endif
