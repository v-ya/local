#define _GNU_SOURCE
#include "multicalc.h"
#include <sched.h>
#include <sys/mman.h>
#include <linux/futex.h>
#include <sys/time.h>
#include <syscall.h>
#include <unistd.h>
#include <stdlib.h>

#define multicalc_stack_dsize  (1ul << 20)
#define multicalc_stack_align  4096ul
#define multicalc_stack_mask   (multicalc_stack_align - 1)

typedef struct multicalc_kernel_t {
	uintptr_t tid;
	multicalc_t *core;
	volatile uintptr_t enable;
	volatile uint32_t started;
	volatile uint32_t stopped;
	volatile multicalc_do_f func;
	void *volatile data;
} multicalc_kernel_t;

struct multicalc_t {
	uint32_t number;
	volatile uint32_t dead;
	void *stack;
	uintptr_t stack_size;
	multicalc_kernel_t kernel[];
};

static int multicalc_kernal_func(multicalc_kernel_t *restrict kernal)
{
	multicalc_t *restrict core = kernal->core;
	multicalc_do_f do_func;
	uint32_t start_status;
	do {
		if (kernal->enable)
		{
			kernal->enable = 0;
			if ((do_func = kernal->func))
				do_func(kernal->data);
			kernal->stopped = 1;
			syscall(SYS_futex, &kernal->stopped, FUTEX_WAKE_PRIVATE, 1, NULL, NULL);
		}
		while ((start_status = kernal->started) != 1)
			syscall(SYS_futex, &kernal->started, FUTEX_WAIT_PRIVATE, start_status, NULL, NULL);
		kernal->started = 2;
	} while (!core->dead);
	kernal->stopped = 1;
	syscall(SYS_futex, &kernal->stopped, FUTEX_WAKE_PRIVATE, 1, NULL, NULL);
	return 0;
}

multicalc_t* multicalc_alloc(uint32_t n, uintptr_t stack_size)
{
	if (n)
	{
		multicalc_t *restrict r;
		uint8_t *restrict stack;
		multicalc_kernel_t *restrict k;
		r = (multicalc_t *) calloc(1, sizeof(multicalc_t) + sizeof(multicalc_kernel_t) * n);
		if (r)
		{
			if (!stack_size)
				stack_size = multicalc_stack_dsize;
			stack_size = (stack_size + multicalc_stack_mask) & ~multicalc_stack_mask;
			stack = mmap(NULL, stack_size * n, (PROT_READ | PROT_WRITE), (MAP_ANONYMOUS | MAP_PRIVATE | MAP_GROWSDOWN), -1, 0);
			if (stack != MAP_FAILED)
			{
				r->stack = stack;
				r->stack_size = stack_size * n;
				k = r->kernel;
				do {
					stack += stack_size;
					k->core = r;
					if (clone((int (*)(void *)) multicalc_kernal_func, stack,
						(CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SYSVSEM | CLONE_SIGHAND | CLONE_THREAD | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID),
						k, &k->tid, NULL, &k->tid) <= 0)
						goto label_fail;
					++k;
				} while(++r->number < n);
				return r;
				label_fail:
				multicalc_free(r);
			}
		}
	}
	return NULL;
}

void multicalc_free(multicalc_t *restrict r)
{
	multicalc_wait(r);
	r->dead = 1;
	multicalc_set_all_status(r, 1);
	multicalc_wake(r);
	multicalc_wait(r);
	if (r->stack_size)
		munmap(r->stack, r->stack_size);
	free(r);
}

void multicalc_set_func(multicalc_t *restrict r, uint32_t index, multicalc_do_f func)
{
	if (index < r->number)
		r->kernel[index].func = func;
}

void multicalc_set_data(multicalc_t *restrict r, uint32_t index, void *data)
{
	if (index < r->number)
		r->kernel[index].data = data;
}

void multicalc_set_status(multicalc_t *restrict r, uint32_t index, uintptr_t enable)
{
	if (index < r->number)
		r->kernel[index].enable = enable;
}

void multicalc_set_all_func(multicalc_t *restrict r, multicalc_do_f func)
{
	multicalc_kernel_t *restrict k;
	uint32_t i;
	k = r->kernel;
	for (i = 0; i < r->number; ++i)
		k[i].func = func;
}

void multicalc_set_all_data(multicalc_t *restrict r, void *data)
{
	multicalc_kernel_t *restrict k;
	uint32_t i;
	k = r->kernel;
	for (i = 0; i < r->number; ++i)
		k[i].data = data;
}

void multicalc_set_all_status(multicalc_t *restrict r, uintptr_t enable)
{
	multicalc_kernel_t *restrict k;
	uint32_t i;
	k = r->kernel;
	for (i = 0; i < r->number; ++i)
		k[i].enable = enable;
}

void multicalc_wake(multicalc_t *restrict r)
{
	multicalc_kernel_t *restrict k;
	uint32_t i;
	k = r->kernel;
	for (i = 0; i < r->number; ++i, ++k)
	{
		if (k->enable && !k->started)
		{
			k->started = 1;
			syscall(SYS_futex, &k->started, FUTEX_WAKE_PRIVATE, 1, NULL, NULL);
		}
	}
}

void multicalc_wait(multicalc_t *restrict r)
{
	multicalc_kernel_t *restrict k;
	uint32_t i, stop_status;
	k = r->kernel;
	for (i = 0; i < r->number; ++i, ++k)
	{
		if (k->started)
		{
			while (!(stop_status = k->stopped))
				syscall(SYS_futex, &k->stopped, FUTEX_WAIT_PRIVATE, stop_status, NULL, NULL);
			k->started = 0;
			k->stopped = 0;
		}
	}
}
