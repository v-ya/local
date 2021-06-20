#ifndef _exbuffer_h_
#define _exbuffer_h_

#include <stdint.h>

typedef struct exbuffer_t {
	uint8_t *data;
	uintptr_t size;
	uintptr_t used;
	uintptr_t cpos;
} exbuffer_t;

exbuffer_t* exbuffer_init(exbuffer_t *restrict eb, uintptr_t dsize);
void exbuffer_uini(exbuffer_t *restrict eb);

exbuffer_t* exbuffer_alloc(uintptr_t dsize);
void exbuffer_free(exbuffer_t *restrict eb);

uintptr_t exbuffer_save(const exbuffer_t *restrict eb);
void exbuffer_load(exbuffer_t *restrict eb, uintptr_t cpos);
void exbuffer_clear(exbuffer_t *restrict eb);

void* exbuffer_need(exbuffer_t *restrict eb, uintptr_t size);
void* exbuffer_mono(exbuffer_t *restrict eb, uintptr_t size, uint32_t log2align, uintptr_t *restrict rpos);
void* exbuffer_monz(exbuffer_t *restrict eb, uintptr_t size, uint32_t log2align, uintptr_t *restrict rpos);

void* exbuffer_append(exbuffer_t *restrict eb, const void *data, uintptr_t size);

void* exbuffer_get_ptr(const exbuffer_t *restrict eb, uintptr_t rpos, uint32_t log2align);
void* exbuffer_curr_ptr(const exbuffer_t *restrict eb, uintptr_t *restrict used);

#endif
