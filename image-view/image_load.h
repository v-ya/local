#ifndef _image_view_image_load_h_
#define _image_view_image_load_h_

#include <refer.h>

typedef struct image_bgra_s {
	const uint32_t *data;
	uintptr_t pixels;
	uint32_t width;
	uint32_t height;
} image_bgra_s;

image_bgra_s* image_bgra_alloc(const char *restrict path);

#endif
