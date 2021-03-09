#define _GNU_SOURCE
#include "slime.h"
#include "syscall.h"

#define slime_mmap_prot    (PROT_READ | PROT_WRITE)
#define slime_mmap_flags   (MAP_ANONYMOUS | MAP_PRIVATE | MAP_GROWSDOWN)
#define slime_clone_flags  (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SYSVSEM | CLONE_SIGHAND | CLONE_THREAD | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID)

slime_attr_t slime_attr = {
	.stack_size = (1 << 20),
	.tls_size   = 0,
	.flags      = 0,
	.tls_data   = NULL
};

static inline slime_attr_t* slime_attr_fix(slime_attr_t *restrict attr)
{
	attr->stack_size = (attr->stack_size >> 12) << 12;
	attr->tls_size = (attr->tls_size >> 4) << 4;
	if (attr->stack_size >= (attr->tls_size + sizeof(slime_t) + sizeof(void *) * 6))
		return attr;
	return NULL;
}

static void slime_finally_pika(struct slime_t *restrict r);
static inline void* slime_init(void *stack, struct slime_t *restrict *restrict p, const slime_attr_t *restrict attr, slime_do_f func, void *arg)
{
	register struct slime_t *restrict r;
	register void **restrict stack_this;
	*p = r = (struct slime_t *) ((uintptr_t) stack + attr->stack_size - sizeof(slime_t));
	r->this = r;
	r->stack_start = stack;
	r->stack_top = r->tls_start = (void *) ((uintptr_t) r - attr->tls_size);
	r->tpid = 0;
	r->attr = *attr;
	stack_this = (void **) r->stack_top;
	*--stack_this = slime_finally_pika;
	*--stack_this = slime_finally_get();
	*--stack_this = func;
	*--stack_this = arg;
	*--stack_this = r;
	*--stack_this = slime_initial_get();
	return stack_this;
}

slime_t* slime_motto(slime_do_f func, void *arg, const slime_attr_t *restrict attr)
{
	struct slime_t *r;
	void *restrict stack_start;
	slime_attr_t ta;
	if (attr) ta = *attr;
	else ta = slime_attr;
	if (slime_attr_fix(&ta))
	{
		stack_start = syscall_mmap(NULL, ta.stack_size, slime_mmap_prot, slime_mmap_flags, -1, 0);
		if ((uintptr_t) stack_start < (-(uintptr_t) 4096))
		{
			stack_start = slime_init(stack_start, &r, &ta, func, arg);
			if (syscall_clone(slime_clone_flags, stack_start, &r->tpid, &r->tpid, NULL) > 0)
				return r;
			syscall_munmap(r->stack_start, ta.stack_size);
		}
	}
	return NULL;
}

void slime_catch(slime_t *restrict slime, int *restrict rstatus)
{
	uintptr_t tpid;
	if (!(slime->attr.flags & slime_flag$detach))
	{
		while ((tpid = (volatile uintptr_t) slime->tpid))
			syscall_futex((uintptr_t *) &slime->tpid, FUTEX_WAIT_PRIVATE, tpid, NULL, NULL, 0);
	}
}

void slime_byebye(slime_t *restrict slime)
{
	if (!(slime->attr.flags & slime_flag$detach))
	{
		if (slime->tpid) slime_catch(slime, NULL);
		syscall_munmap(slime->stack_start, slime->attr.stack_size);
	}
}

static void slime_finally_pika(struct slime_t *restrict r)
{
	if (r->attr.flags & slime_flag$detach)
		syscall_munmap(r->stack_start, r->attr.stack_size);
	else
	{
		r->tpid = 0;
		syscall_futex(&r->tpid, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
	}
	syscall_exit(0);
}
