#define _GNU_SOURCE
#include "slime.h"
#include "dtv.h"
#include "syscall.h"
#include "define.h"

#define slime_mmap_prot    (PROT_READ | PROT_WRITE)
#define slime_mmap_flags   (MAP_ANONYMOUS | MAP_PRIVATE | MAP_GROWSDOWN)
#define slime_clone_flags  (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SYSVSEM | CLONE_SIGHAND | CLONE_THREAD | CLONE_PARENT_SETTID | CLONE_SETTLS | CLONE_CHILD_CLEARTID)

#define size_algin(_size, _align)  (((_size) + (_align - 1)) & ~(_align - 1))

struct slime_t {
	slime_t *this;
	void *box_start;
	void *stack_start;
	void *tls_save_pos;
	void **stack_top;
	uintptr_t tpid;
	slime_attr_t attr;
};

slime_attr_t slime_attr = {
	.stack_size = (1 << 20),
	.flags      = 0,
	.box_size   = 0,
	.box_data   = NULL
};

static inline slime_attr_t* slime_attr_fix(slime_attr_t *restrict attr)
{
	attr->stack_size = (attr->stack_size >> 12) << 12;
	if (attr->stack_size >= (size_algin(attr->box_size, 16) + sizeof(slime_t) + wtf_pthread_res_size + wtf_tls_static_size + sizeof(void *) * 16))
		return attr;
	return NULL;
}

static void slime_finally_pika(struct slime_t *restrict r);
static inline void* slime_init(void *stack, struct slime_t *restrict *restrict p, const slime_attr_t *restrict attr, slime_do_f func, void *arg)
{
	register struct slime_t *restrict r;
	register void **restrict stack_this;
	void *dynamic_tls_vlist;
	*p = r = (struct slime_t *) ((uintptr_t) stack + attr->stack_size - sizeof(slime_t));
	r->this = r;
	r->box_start = NULL;
	r->stack_start = stack;
	r->tls_save_pos = (void *) ((uintptr_t) r - wtf_pthread_res_size);
	r->stack_top = NULL;
	r->tpid = 0;
	r->attr = *attr;
	r->attr.box_data = NULL;
	if ((dynamic_tls_vlist = gnu_linux_amd64_dtv_alloc()))
	{
		// set tls_save_pos
		stack_this = (void **) r->tls_save_pos;
		inner_mclear((void *) ((uintptr_t) stack_this - wtf_tls_static_size), wtf_pthread_res_size + wtf_tls_static_size);
		stack_this[0] = (void *) stack_this;
		stack_this[1] = dynamic_tls_vlist;
		// skip tls_static_size
		stack_this = (void **) ((uintptr_t) stack_this - wtf_tls_static_size);
		// set this (%fs:-8)
		*--stack_this = r;
		*--stack_this = NULL;
		// set tls start
		if (r->attr.box_size)
		{
			r->box_start = (void *) ((uintptr_t) stack_this - size_algin(r->attr.box_size, 16));
			stack_this = (void **) r->box_start;
			if (attr->box_data)
				inner_mcopy(r->box_start, attr->box_data, r->attr.box_size);
			else inner_mclear(r->box_start, r->attr.box_size);
		}
		// set stack top
		r->stack_top = stack_this;
		*--stack_this = slime_finally_pika;
		*--stack_this = slime_finally_get();
		*--stack_this = func;
		*--stack_this = arg;
		*--stack_this = r;
		*--stack_this = slime_initial_get();
		return stack_this;
	}
	return NULL;
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
			if (stack_start && syscall_clone(slime_clone_flags, stack_start, &r->tpid, &r->tpid, r->tls_save_pos) > 0)
				return r;
			syscall_munmap(r->stack_start, ta.stack_size);
		}
	}
	return NULL;
}

void slime_catch(slime_t *restrict slime)
{
	uintptr_t tpid;
	if (!(slime->attr.flags & slime_flag$detach))
	{
		while ((tpid = (volatile uintptr_t) slime->tpid))
			syscall_futex((uintptr_t *) &slime->tpid, FUTEX_WAIT_PRIVATE, tpid, NULL, NULL, 0);
	}
}

static void slime_explosion(slime_t *restrict r);
void slime_byebye(slime_t *restrict slime)
{
	if (!(slime->attr.flags & slime_flag$detach))
	{
		if (slime->tpid) slime_catch(slime);
		slime_explosion(slime);
	}
}

slime_t* slime_me(void)
{
	return get_self();
}

void* slime_box(slime_t *restrict slime, uintptr_t *restrict size)
{
	if (size) *size = slime->attr.box_size;
	return slime->box_start;
}

static inline uintptr_t slime_box_fix(uintptr_t want, uintptr_t offset, uintptr_t size)
{
	if (size > offset)
	{
		size -= offset;
		return (want <= size)?want:size;
	}
	return 0;
}

uintptr_t slime_box_put(slime_t *restrict slime, uintptr_t offset, const void *data, uintptr_t size)
{
	if ((size = slime_box_fix(size, offset, slime->attr.box_size)))
		inner_mcopy((void *) ((uintptr_t) slime->box_start + offset), data, size);
	return size;
}

uintptr_t slime_box_get(slime_t *restrict slime, uintptr_t offset, void *data, uintptr_t size)
{
	if ((size = slime_box_fix(size, offset, slime->attr.box_size)))
		inner_mcopy(data, (const void *) ((uintptr_t) slime->box_start + offset), size);
	return size;
}

static void slime_explosion(slime_t *restrict r)
{
	register void *dynamic_tls_vlist;
	if ((dynamic_tls_vlist = ((void **) r->tls_save_pos)[1]))
		gnu_linux_amd64_dtv_free(dynamic_tls_vlist);
	syscall_munmap(r->stack_start, r->attr.stack_size);
}

static void slime_finally_pika(struct slime_t *restrict r)
{
	if (r->attr.flags & slime_flag$detach)
		slime_explosion(r);
	else
	{
		r->tpid = 0;
		syscall_futex(&r->tpid, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
	}
	syscall_exit(0);
}
