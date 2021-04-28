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

#define mpeg4_fix_point_gen(_n, _t, _a, _b)  mpeg4$define(inner, f2i, _t)((_n) * (1ul << _b))

#endif
