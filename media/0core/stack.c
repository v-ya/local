#include "stack.h"
#include <memory.h>

static void media_stack_free_func(struct media_stack_s *restrict r)
{
	media_stack_clear(r);
	exbuffer_uini(&r->stack);
}

struct media_stack_s* media_stack_alloc(const struct media_stack_param_t *restrict param)
{
	struct media_stack_s *restrict r;
	uintptr_t size_block;
	uintptr_t size_used;
	size_block = param->stack_align;
	size_used = param->stack_size;
	size_block = (size_block > 1)?((size_used + size_block - 1) / size_block * size_block):size_used;
	if (size_used && size_block && (r = (struct media_stack_s *) refer_alloz(sizeof(struct media_stack_s))))
	{
		if (exbuffer_init(&r->stack, 0))
		{
			refer_set_free(r, (refer_free_f) media_stack_free_func);
			r->copy = param->copy;
			r->clear = param->clear;
			r->size_block = size_block;
			r->size_used = size_used;
			r->layout_magic = param->layout_magic;
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
