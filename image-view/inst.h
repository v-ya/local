#ifndef _image_view_inst_h_
#define _image_view_inst_h_

#include "window.h"
#include "image_load.h"
#include "image_resample.h"

typedef struct inst_s {
	const image_bgra_s *image;
	image_resample_s *resample;
	window_s *window;
	volatile uintptr_t is_close;
	volatile uintptr_t update;
	uint32_t width;
	uint32_t height;
	int32_t last_x;
	int32_t last_y;
	uint32_t b_tran;
	uint32_t b_rota;
} inst_s;

inst_s* inst_alloc(const char *restrict path);
void inst_free(inst_s *restrict r);
void inst_wait(inst_s *restrict r);

#endif
