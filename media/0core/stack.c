#include "stack.h"
#include <memory.h>

static void media_stack_free_func(struct media_stack_s *restrict r)
{
	media_stack_clear(r);
	exbuffer_uini(&r->stack);
}

struct media_stack_s* media_stack_alloc(uintptr_t stack_size, uintptr_t stack_align, media_stack_copy_f copy, media_stack_clear_f clear)
{
	struct media_stack_s *restrict r;
	stack_align = (stack_align > 1)?((stack_size + stack_size - 1) / stack_size * stack_size):stack_size;
	if (stack_size && stack_align && (r = (struct media_stack_s *) refer_alloz(sizeof(struct media_stack_s))))
	{
		if (exbuffer_init(&r->stack, 0))
		{
			refer_set_free(r, (refer_free_f) media_stack_free_func);
			r->copy = copy;
			r->clear = clear;
			r->size_block = stack_align;
			r->size_used = stack_size;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

void media_stack_clear(struct media_stack_s *restrict stack)
{
	media_stack_clear_f clear;
	uint8_t *b, *e;
	uintptr_t z;
	e = (b = stack->stack.data) + (z = stack->size_block) * stack->stack_number;
	if (e > b)
	{
		if ((clear = stack->clear))
		{
			do clear(e -= z);
			while (e > b);
		}
		stack->stack_number = 0;
	}
}

struct media_stack_s* media_stack_push(struct media_stack_s *restrict stack, const void *restrict p)
{
	uint8_t *restrict d;
	uintptr_t pos, z;
	media_stack_copy_f copy;
	pos = (z = stack->size_block) * stack->stack_number;
	if ((d = exbuffer_need(&stack->stack, pos + z)))
	{
		memcpy(d += pos, p, stack->size_used);
		if (!(copy = stack->copy) || copy(d))
		{
			stack->stack_number += 1;
			return stack;
		}
	}
	return NULL;
}

void media_stack_pop(struct media_stack_s *restrict stack)
{
	media_stack_clear_f clear;
	uintptr_t n;
	if ((n = stack->stack_number))
	{
		stack->stack_number = (n -= 1);
		if ((clear = stack->clear))
			clear(stack->stack.data + stack->size_block * n);
	}
}

struct media_stack_s* media_stack_set_index(struct media_stack_s *restrict stack, uintptr_t index)
{
	if (index < stack->stack_number)
	{
		stack->stack_index = index;
		return stack;
	}
	return NULL;
}

const void* media_stack_get(struct media_stack_s *restrict stack, uintptr_t index)
{
	if (index < stack->stack_number)
		return stack->stack.data + stack->size_block * index;
	return NULL;
}

const void* media_stack_get_and_next(struct media_stack_s *restrict stack)
{
	uintptr_t i;
	if ((i = stack->stack_index) < stack->stack_number)
	{
		stack->stack_index = i + 1;
		return stack->stack.data + stack->size_block * i;
	}
	return NULL;
}
