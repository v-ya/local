#include "inline_futex.h"
#include "yaw.h"

typedef struct yaw_lock_mutex_s {
	yaw_lock_s ym;
	volatile uint32_t status;
} yaw_lock_mutex_s;

#define me(_r)  ((yaw_lock_mutex_s *) _r)

static uint32_t yaw_lock_mutex_test_func(volatile uint32_t *address, uint32_t waitat)
{
	return __sync_val_compare_and_swap(address, 0, 1);
}

static void yaw_lock_mutex_lock(yaw_lock_s *r)
{
	if (__sync_val_compare_and_swap(&me(r)->status, 0, 1))
		yaw_inline_futex_wait_okay(&me(r)->status, 1, yaw_lock_mutex_test_func);
}

static void yaw_lock_mutex_unlock(yaw_lock_s *r)
{
	__sync_lock_test_and_set(&me(r)->status, 0);
	yaw_inline_futex_wake(&me(r)->status, 1);
}

static yaw_lock_s* yaw_lock_mutex_trylock(yaw_lock_s *r)
{
	if (!__sync_val_compare_and_swap(&me(r)->status, 0, 1))
		return r;
	return NULL;
}

static yaw_lock_s* yaw_lock_mutex_waitlock(yaw_lock_s *r, uintptr_t usec)
{
	if (yaw_inline_futex_wait_time(&me(r)->status, 1, yaw_lock_mutex_test_func, usec))
		return r;
	return NULL;
}

yaw_lock_s* yaw_lock_alloc_mutex(void)
{
	yaw_lock_mutex_s *restrict r;
	if ((r = (yaw_lock_mutex_s *) refer_alloc(sizeof(yaw_lock_mutex_s))))
	{
		r->ym.lock = yaw_lock_mutex_lock;
		r->ym.unlock = yaw_lock_mutex_unlock;
		r->ym.trylock = yaw_lock_mutex_trylock;
		r->ym.waitlock = yaw_lock_mutex_waitlock;
		r->status = 0;
	}
	return &r->ym;
}
