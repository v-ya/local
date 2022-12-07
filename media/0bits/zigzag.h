#ifndef _media_bits_zigzag_h_
#define _media_bits_zigzag_h_

#include <refer.h>

// 2d

struct media_zigzag_s {
	uintptr_t w;
	uintptr_t h;
	const uintptr_t *i2s;
	const uintptr_t *s2i;
};

struct media_zigzag_s* media_zigzag_alloc(uintptr_t w, uintptr_t h);

#endif
