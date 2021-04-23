#ifndef _mpeg4_inner_data_h_
#define _mpeg4_inner_data_h_

#include "box.include.h"

void* mpeg4$define(inner, data, get)(void *restrict r, size_t rsize, const uint8_t *restrict *restrict data, uint64_t *restrict size);
uint32_t* mpeg4$define(inner, uint32_t, get)(uint32_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size);

#endif
