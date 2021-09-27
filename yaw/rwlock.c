#define _DEFAULT_SOURCE
#include "yaw.h"
#include <pthread.h>

typedef struct yaw_lock_rwlock_s {
	yaw_lock_s ym;
	pthread_rwlock_t *rwlock;
} yaw_lock_rwlock_s;

#define me(_r)  ((yaw_lock_rwlock_s *) _r)

static void yaw_lock_rwlock_rwlock_free(pthread_rwlock_t *restrict r)
{
	pthread_rwlock_destroy(r);
}

static void yaw_lock_rwlock_free(yaw_lock_rwlock_s *restrict r)
{
	if (r->rwlock)
		refer_free(r->rwlock);
}

static void yaw_lock_rwlock_lock_read(yaw_lock_s *r)
{
	pthread_rwlock_rdlock(me(r)->rwlock);
}

static void yaw_lock_rwlock_lock_write(yaw_lock_s *r)
{
	pthread_rwlock_wrlock(me(r)->rwlock);
}

static void yaw_lock_rwlock_unlock(yaw_lock_s *r)
{
	pthread_rwlock_unlock(me(r)->rwlock);
}

static yaw_lock_s* yaw_lock_rwlock_trylock_read(yaw_lock_s *r)
{
	if (!pthread_rwlock_tryrdlock(me(r)->rwlock))
		return r;
	return NULL;
}

static yaw_lock_s* yaw_lock_rwlock_trylock_write(yaw_lock_s *r)
{
	if (!pthread_rwlock_trywrlock(me(r)->rwlock))
		return r;
	return NULL;
}

static yaw_lock_s* yaw_lock_rwlock_waitlock_read(yaw_lock_s *r, uintptr_t usec)
{
	struct timespec ts = {
		.tv_sec = usec / 1000000,
		.tv_nsec = (usec % 1000000) * 1000
	};
	if (!pthread_rwlock_timedrdlock(me(r)->rwlock, &ts))
		return r;
	return NULL;
}

static yaw_lock_s* yaw_lock_rwlock_waitlock_write(yaw_lock_s *r, uintptr_t usec)
{
	struct timespec ts = {
		.tv_sec = usec / 1000000,
		.tv_nsec = (usec % 1000000) * 1000
	};
	if (!pthread_rwlock_timedwrlock(me(r)->rwlock, &ts))
		return r;
	return NULL;
}

int yaw_lock_alloc_rwlock(yaw_lock_s *restrict *restrict read, yaw_lock_s *restrict *restrict write)
{
	pthread_rwlock_t *rwlock;
	yaw_lock_rwlock_s *restrict r, *restrict w;
	int ret;
	r = w = NULL;
	ret = -1;
	if (read && write)
	{
		if ((rwlock = (pthread_rwlock_t *) refer_alloc(sizeof(pthread_rwlock_t))))
		{
			if (!pthread_rwlock_init(rwlock, NULL))
			{
				refer_set_free(rwlock, (refer_free_f) yaw_lock_rwlock_rwlock_free);
				r = (yaw_lock_rwlock_s *) refer_alloc(sizeof(yaw_lock_rwlock_s));
				w = (yaw_lock_rwlock_s *) refer_alloc(sizeof(yaw_lock_rwlock_s));
				if (r && w)
				{
					r->rwlock = (pthread_rwlock_t *) refer_save(rwlock);
					w->rwlock = (pthread_rwlock_t *) refer_save(rwlock);
					refer_set_free(r, (refer_free_f) yaw_lock_rwlock_free);
					refer_set_free(w, (refer_free_f) yaw_lock_rwlock_free);
					// read
					r->ym.lock = yaw_lock_rwlock_lock_read;
					r->ym.unlock = yaw_lock_rwlock_unlock;
					r->ym.trylock = yaw_lock_rwlock_trylock_read;
					r->ym.waitlock = yaw_lock_rwlock_waitlock_read;
					// write
					r->ym.lock = yaw_lock_rwlock_lock_write;
					r->ym.unlock = yaw_lock_rwlock_unlock;
					r->ym.trylock = yaw_lock_rwlock_trylock_write;
					r->ym.waitlock = yaw_lock_rwlock_waitlock_write;
					// ret
					ret = 0;
				}
				else
				{
					if (r) refer_free(r);
					if (w) refer_free(w);
					r = w = NULL;
				}
			}
			refer_free(rwlock);
		}
	}
	if (read) *read = &r->ym;
	if (write) *write = &w->ym;
	return ret;
}
