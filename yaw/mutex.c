#define _DEFAULT_SOURCE
#include "yaw.h"
#include <pthread.h>

typedef struct yaw_lock_mutex_s {
	yaw_lock_s ym;
	pthread_mutex_t mutex;
} yaw_lock_mutex_s;

#define me(_r)  ((yaw_lock_mutex_s *) _r)

static void yaw_lock_mutex_free(yaw_lock_mutex_s *restrict r)
{
	pthread_mutex_destroy(&r->mutex);
}

static void yaw_lock_mutex_lock(yaw_lock_s *r)
{
	pthread_mutex_lock(&me(r)->mutex);
}

static void yaw_lock_mutex_unlock(yaw_lock_s *r)
{
	pthread_mutex_unlock(&me(r)->mutex);
}

static yaw_lock_s* yaw_lock_mutex_trylock(yaw_lock_s *r)
{
	if (!pthread_mutex_trylock(&me(r)->mutex))
		return r;
	return NULL;
}

static yaw_lock_s* yaw_lock_mutex_waitlock(yaw_lock_s *r, uintptr_t usec)
{
	struct timespec ts = {
		.tv_sec = usec / 1000000,
		.tv_nsec = (usec % 1000000) * 1000
	};
	if (!pthread_mutex_timedlock(&me(r)->mutex, &ts))
		return r;
	return NULL;
}

yaw_lock_s* yaw_lock_alloc_mutex(void)
{
	yaw_lock_mutex_s *restrict r;
	if ((r = (yaw_lock_mutex_s *) refer_alloc(sizeof(yaw_lock_mutex_s))))
	{
		if (!pthread_mutex_init(&r->mutex, NULL))
		{
			refer_set_free(r, (refer_free_f) yaw_lock_mutex_free);
			r->ym.lock = yaw_lock_mutex_lock;
			r->ym.unlock = yaw_lock_mutex_unlock;
			r->ym.trylock = yaw_lock_mutex_trylock;
			r->ym.waitlock = yaw_lock_mutex_waitlock;
			return &r->ym;
		}
		refer_free(r);
	}
	return NULL;
}
