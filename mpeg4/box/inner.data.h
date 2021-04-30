#ifndef _mpeg4_inner_data_h_
#define _mpeg4_inner_data_h_

#include "box.include.h"

void* mpeg4$define(inner, data, get)(void *restrict r, size_t rsize, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, data, set)(uint8_t *restrict data, void *restrict v, size_t vsize);

uint32_t* mpeg4$define(inner, uint32_t, get)(uint32_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, uint32_t, set)(uint8_t *restrict data, uint32_t v);

int16_t mpeg4$define(inner, f2i, int16_t)(double v);
uint16_t mpeg4$define(inner, f2i, uint16_t)(double v);
int32_t mpeg4$define(inner, f2i, int32_t)(double v);
uint32_t mpeg4$define(inner, f2i, uint32_t)(double v);

char** mpeg4$define(inner, string, set)(char **restrict p_string, uintptr_t *restrict p_length, const char *restrict string);

typedef struct inner_array_t {
	uint8_t *array;
	const uintptr_t item_size;
	uintptr_t size;
	uintptr_t number;
} inner_array_t;

void mpeg4$define(inner, array, init)(inner_array_t *restrict array, uintptr_t item_size);
void mpeg4$define(inner, array, clear)(inner_array_t *restrict array);
void* mpeg4$define(inner, array, append_point)(inner_array_t *restrict array, uintptr_t n);
void* mpeg4$define(inner, array, append_data)(inner_array_t *restrict array, uintptr_t n, const void *data);

#define mpeg4_fix_point_gen(_n, _t, _a, _b)  mpeg4$define(inner, f2i, _t)((_n) * (1ul << _b))

#endif
