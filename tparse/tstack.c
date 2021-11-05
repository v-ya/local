#include "tstack.h"
#include <exbuffer.h>

typedef struct tparse_tstack_backup_t {
	tparse_tstack_free_f free;
	uintptr_t cpos_last;
	uintptr_t rpos_curr;
	uintptr_t size;
} tparse_tstack_backup_t;

struct tparse_tstack_s {
	exbuffer_t stack;
	exbuffer_t backup;
	// config
	uintptr_t depth_max_backup_size;
	uint32_t log2align;
};

static void tparse_tstack_free_func(tparse_tstack_s *restrict r)
{
	tparse_tstack_clear(r);
	exbuffer_uini(&r->stack);
	exbuffer_uini(&r->backup);
}

tparse_tstack_s* tparse_tstack_alloc(void)
{
	tparse_tstack_s *restrict r;
	r = (tparse_tstack_s *) refer_alloz(sizeof(tparse_tstack_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) tparse_tstack_free_func);
		if (exbuffer_init(&r->stack, 0) && exbuffer_init(&r->backup, 0))
		{
			r->depth_max_backup_size = ~(uintptr_t) 0;
			r->log2align = 4;
		}
		refer_free(r);
	}
	return NULL;
}

uintptr_t tparse_tstack_layer_number(tparse_tstack_s *restrict stack)
{
	return stack->backup.used / sizeof(tparse_tstack_backup_t);
}

void* tparse_tstack_layer(tparse_tstack_s *restrict stack, uintptr_t layer_rpos, uintptr_t *restrict rsize)
{
	tparse_tstack_backup_t *restrict p;
	uintptr_t n;
	if ((n = stack->backup.used / sizeof(tparse_tstack_backup_t)) > layer_rpos)
	{
		n = n - layer_rpos - 1;
		p = (tparse_tstack_backup_t *) stack->backup.data;
		if (rsize) *rsize = p[n].size;
		return exbuffer_get_ptr(&stack->stack, p[n].rpos_curr, 0);
	}
	return NULL;
}

void* tparse_tstack_push(tparse_tstack_s *restrict stack, uintptr_t size, tparse_tstack_free_f free_func)
{
	tparse_tstack_backup_t backup;
	void *r;
	if (stack->backup.used < stack->depth_max_backup_size && size)
	{
		backup.free = free_func;
		backup.cpos_last = stack->stack.cpos;
		backup.rpos_curr = 0;
		backup.size = size;
		if ((r = exbuffer_monz(&stack->stack, size, stack->log2align, &backup.rpos_curr)))
		{
			if (exbuffer_append(&stack->backup, &backup, sizeof(backup)))
				return r;
			stack->stack.cpos = backup.cpos_last;
		}
	}
	return NULL;
}

void tparse_tstack_pop(tparse_tstack_s *restrict stack)
{
	tparse_tstack_backup_t *restrict p;
	uintptr_t n;
	if ((n = stack->backup.used / sizeof(tparse_tstack_backup_t)))
	{
		--n;
		p = (tparse_tstack_backup_t *) stack->backup.data;
		if (p[n].free)
			p[n].free(exbuffer_get_ptr(&stack->stack, p[n].rpos_curr, 0));
		exbuffer_load(&stack->stack, p[n].cpos_last);
		stack->backup.used = n * sizeof(tparse_tstack_backup_t);
	}
}

void tparse_tstack_clear(tparse_tstack_s *restrict stack)
{
	while (stack->backup.used)
		tparse_tstack_pop(stack);
}

