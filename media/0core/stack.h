#ifndef _media_core_stack_h_
#define _media_core_stack_h_

#include <refer.h>
#include <exbuffer.h>

typedef void* (*media_stack_copy_f)(void *restrict p);
typedef void (*media_stack_clear_f)(void *restrict p);

struct media_stack_s {
	exbuffer_t stack;
	media_stack_copy_f copy;
	media_stack_clear_f clear;
	uintptr_t size_block;
	uintptr_t size_used;
	uintptr_t stack_number;
	uintptr_t stack_index;
};

struct media_stack_s* media_stack_alloc(uintptr_t stack_size, uintptr_t stack_align, media_stack_copy_f copy, media_stack_clear_f clear);
void media_stack_clear(struct media_stack_s *restrict stack);
struct media_stack_s* media_stack_push(struct media_stack_s *restrict stack, const void *restrict p);
void media_stack_pop(struct media_stack_s *restrict stack);
struct media_stack_s* media_stack_set_index(struct media_stack_s *restrict stack, uintptr_t index);
const void* media_stack_get(struct media_stack_s *restrict stack, uintptr_t index);
const void* media_stack_get_and_next(struct media_stack_s *restrict stack);

#endif
