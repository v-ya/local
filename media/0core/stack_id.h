#ifndef _media_core_stack_id_h_
#define _media_core_stack_id_h_

#include <refer.h>
#include "stack.h"

typedef const char* (*media_stack_layout_initial_f)(struct media_stack_param_t *restrict param);

struct media_stack_id_s {
	refer_string_t name;
	struct media_stack_param_t param;
};

struct media_stack_id_s* media_stack_id_alloc(media_stack_layout_initial_f initial);

// layout ...

// o: offset (uint64_t)
// z: size   (uintptr_t)

#define media_sl_oz  "sl/oz"

enum media_stack_layout_t {
	media_stack_layout__oz,
};

// oz
struct media_stack__oz_t {
	uint64_t offset;
	uintptr_t size;
};
const char* media_stack_layout_initial__oz(struct media_stack_param_t *restrict param);

#endif
