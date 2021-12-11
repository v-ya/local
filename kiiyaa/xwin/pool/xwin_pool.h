#ifndef _kiiyaa_xwin_pool__xwin_pool_h_
#define _kiiyaa_xwin_pool__xwin_pool_h_

#include <xwindow.h>

typedef struct xwin_pool_s xwin_pool_s;

typedef refer_t (*xwin_pool_data_allocer_f)(xwindow_s *restrict xw);
typedef xwindow_s* (*xwin_pool_events_done_f)(xwindow_s *restrict xw, refer_t data);

typedef struct xwin_pool_param_t {
	int32_t x;
	int32_t y;
	uint32_t w;
	uint32_t h;
	uint32_t depth;
	uint32_t enable_shm;
	uintptr_t shm_size;
	uintptr_t events_time_gap_msec;
	const char *title;
	const uint32_t *icon_data;
	uint32_t icon_width;
	uint32_t icon_height;
	xwin_pool_data_allocer_f data_allocer;
	xwin_pool_events_done_f  events_done_func;
	xwindow_event_close_f    event_close_func;
	xwindow_event_expose_f   event_expose_func;
	xwindow_event_key_f      event_key_func;
	xwindow_event_button_f   event_button_func;
	xwindow_event_pointer_f  event_pointer_func;
	xwindow_event_area_f     event_area_func;
	xwindow_event_focus_f    event_focus_func;
	xwindow_event_config_f   event_config_func;
} xwin_pool_param_t;

xwin_pool_s* xwin_pool_alloc(void);
xwin_pool_s* xwin_pool_add_xwin(xwin_pool_s *xp, const char *restrict name, const xwin_pool_param_t *restrict param);

#endif
