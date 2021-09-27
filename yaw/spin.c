#define _DEFAULT_SOURCE
#include "yaw.h"
#include <pthread.h>

typedef struct yaw_lock_spin_s {
	yaw_lock_s ym;
	pthread_spinlock_t spin;
} yaw_lock_spin_s;

#define me(_r)  ((yaw_lock_spin_s *) _r)

static void yaw_lock_spin_free(yaw_lock_spin_s *restrict r)
{
	pthread_spin_destroy(&r->spin);
}

static void yaw_lock_spin_lock(yaw_lock_s *r)
{
	pthread_spin_lock(&me(r)->spin);
}

static void yaw_lock_spin_unlock(yaw_lock_s *r)
{
	pthread_spin_unlock(&me(r)->spin);
}

static yaw_lock_s* yaw_lock_spin_trylock(yaw_lock_s *r)
{
	if (!pthread_spin_trylock(&me(r)->spin))
		return r;
	return NULL;
}

static yaw_lock_s* yaw_lock_spin_waitlock(yaw_lock_s *r, uintptr_t usec)
{
	if (!pthread_spin_trylock(&me(r)->spin))
		return r;
	return NULL;
}

yaw_lock_s* yaw_lock_alloc_spin(void)
{
	yaw_lock_spin_s *restrict r;
	if ((r = (yaw_lock_spin_s *) refer_alloc(sizeof(yaw_lock_spin_s))))
	{
		if (!pthread_spin_init(&r->spin, PTHREAD_PROCESS_PRIVATE))
		{
			refer_set_free(r, (refer_free_f) yaw_lock_spin_free);
			r->ym.lock = yaw_lock_spin_lock;
			r->ym.unlock = yaw_lock_spin_unlock;
			r->ym.trylock = yaw_lock_spin_trylock;
			r->ym.waitlock = yaw_lock_spin_waitlock;
			return &r->ym;
		}
		refer_free(r);
	}
	return NULL;
}
