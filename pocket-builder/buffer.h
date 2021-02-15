#ifndef _buffer_h_
#define _buffer_h_

#include <stdint.h>
#include <stddef.h>

typedef struct buffer_t {
	uint8_t *data;
	uintptr_t size;
	uintptr_t used;
} buffer_t;

buffer_t* buffer_alloc(void);
void buffer_free(buffer_t *restrict buffer);
void buffer_clear(buffer_t *restrict buffer);
buffer_t* buffer_need(buffer_t *restrict buffer, uintptr_t size);

#endif
