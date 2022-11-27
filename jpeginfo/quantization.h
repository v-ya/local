#ifndef _quantization_h_
#define _quantization_h_

#include <refer.h>

typedef struct quantization_s {
	uint32_t q[64];
} quantization_s;

quantization_s* quantization_alloc(uintptr_t depth, const void *restrict q64);

#endif
