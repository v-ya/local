#include "yaw.h"

typedef struct yaw_lock_spin_s {
	yaw_lock_s ym;
	volatile uint32_t status;
} yaw_lock_spin_s;

#define me(_r)  ((yaw_lock_spin_s *) _r)

static void yaw_lock_spin_lock(yaw_lock_s *r)
{
	while (__sync_val_compare_and_swap(&me(r)->status, 0, 1));
}

static void yaw_lock_spin_unlock(yaw_lock_s *r)
{
	__sync_lock_test_and_set(&me(r)->status, 0);
}

static yaw_lock_s* yaw_lock_spin_trylock(yaw_lock_s *r)
{
	if (!__sync_val_compare_and_swap(&me(r)->status, 0, 1))
		return r;
	return NULL;
}

static yaw_lock_s* yaw_lock_spin_waitlock(yaw_lock_s *r, uintptr_t usec)
{
	return yaw_lock_spin_trylock(r);
}

yaw_lock_s* yaw_lock_alloc_spin(void)
{
	yaw_lock_spin_s *restrict r;
	if ((r = (yaw_lock_spin_s *) refer_alloc(sizeof(yaw_lock_spin_s))))
	{
		r->ym.lock = yaw_lock_spin_lock;
		r->ym.unlock = yaw_lock_spin_unlock;
		r->ym.trylock = yaw_lock_spin_trylock;
		r->ym.waitlock = yaw_lock_spin_waitlock;
		r->status = 0;
	}
	return &r->ym;
}
