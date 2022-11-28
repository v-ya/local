#ifndef _display_h_
#define _display_h_

#include <refer.h>
#include <xwindow.h>
#include <yaw.h>

typedef struct display_s {
	xwindow_s *xw;
	xwindow_image_s *image;
	yaw_s *yaw;
} display_s;

display_s* display_alloc(uint32_t width, uint32_t height);
void display_wait_close(display_s *restrict d);

#endif
