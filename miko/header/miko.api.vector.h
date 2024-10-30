#ifndef _miko_api_vector_h_
#define _miko_api_vector_h_

#include "miko.type.vector.h"

#define miko_vector_auto  (~(uintptr_t) 0)

miko_vector_s_t miko_vector_alloc(uintptr_t item_size, miko_vector_initial_f initial, miko_vector_finally_f finally);
uintptr_t miko_vector_push(miko_vector_s_t r, const void *item_array, uintptr_t item_count);
uintptr_t miko_vector_pop(miko_vector_s_t r, uintptr_t item_count);
uintptr_t miko_vector_count(const miko_vector_s_t r);
void* miko_vector_data(const miko_vector_s_t r);
void* miko_vector_index(const miko_vector_s_t r, uintptr_t item_index);

// (item_offset == miko_vector_auto) => (item_offset = vector.count - item_count)
// (item_count  == miko_vector_auto) => (item_count = vector.count - item_offset)
// if (return != NULL) *count must > 0
void* miko_vector_map(const miko_vector_s_t r, uintptr_t item_offset, uintptr_t item_count, uintptr_t *restrict count);

#endif
