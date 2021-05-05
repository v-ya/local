#ifndef _mpeg4_inner_data_h_
#define _mpeg4_inner_data_h_

#include "box.include.h"

void* mpeg4$define(inner, data, get)(void *restrict r, size_t rsize, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, data, set)(uint8_t *restrict data, void *restrict v, size_t vsize);

uint8_t* mpeg4$define(inner, uint8_t, get)(uint8_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, uint8_t, set)(uint8_t *restrict data, uint8_t v);

uint16_t* mpeg4$define(inner, uint16_t, get)(uint16_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, uint16_t, set)(uint8_t *restrict data, uint16_t v);

int16_t* mpeg4$define(inner, int16_t, get)(int16_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, int16_t, set)(uint8_t *restrict data, int16_t v);

uint32_t* mpeg4$define(inner, uint24_t, get)(uint32_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, uint24_t, set)(uint8_t *restrict data, uint32_t v);

uint32_t* mpeg4$define(inner, uint32_t, get)(uint32_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint8_t* mpeg4$define(inner, uint32_t, set)(uint8_t *restrict data, uint32_t v);

int16_t mpeg4$define(inner, f2i, int16_t)(double v);
uint16_t mpeg4$define(inner, f2i, uint16_t)(double v);
int32_t mpeg4$define(inner, f2i, int32_t)(double v);
uint32_t mpeg4$define(inner, f2i, uint32_t)(double v);

char** mpeg4$define(inner, string, set_with_length)(char **restrict p_string, uintptr_t *restrict p_length, const char *restrict string, uintptr_t n);
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

#define inner_method_set_simple_param(_n, _m, _tn, _tm, _p) \
	const mpeg4_stuff_t* mpeg4$define(stuff, _n, set$##_m)(_tn *restrict r, _tm _m)\
	{\
		r->_p = _m;\
		return &r->stuff;\
	}

#define inner_method_get_simple_param(_n, _m, _tn, _tm, _p) \
	const mpeg4_stuff_t* mpeg4$define(stuff, _n, get$##_m)(_tn *restrict r, _tm *restrict _m)\
	{\
		if (_m) *_m = r->_p;\
		return &r->stuff;\
	}

#endif
