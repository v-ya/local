#include "inline_futex.h"
#include "yaw.h"

typedef struct yaw_rwlock_s {
	volatile uint32_t rd_all;       // all read lock number
	volatile uint32_t rd_wait;      // wait read lock number
	volatile uint32_t rd_got;       // got read lock number
	volatile uint32_t wr_request;  // request write lock number
	volatile uint32_t wr_mutex;    // 0: unlock, 1: lock
} yaw_rwlock_s;

typedef struct yaw_lock_rwlock_s {
	yaw_lock_s ym;
	yaw_rwlock_s *rwlock;
} yaw_lock_rwlock_s;

static inline void yaw_rwlock_rd_lock(yaw_rwlock_s *r)
{
	uint32_t n;
	__sync_fetch_and_add(&r->rd_all, 1);
	while ((n = r->wr_request))
	{
		__sync_fetch_and_add(&r->rd_wait, 1);
		yaw_inline_futex_wait(&r->wr_request, n, NULL);
		__sync_fetch_and_sub(&r->rd_wait, 1);
	}
	__sync_fetch_and_add(&r->rd_got, 1);
	return ;
}

static inline void yaw_rwlock_wr_lock(yaw_rwlock_s *r)
{
	uint32_t n;
	__sync_fetch_and_add(&r->wr_request, 1);
	while (__sync_val_compare_and_swap(&r->wr_mutex, 0, 1))
		yaw_inline_futex_wait(&r->wr_mutex, 1, NULL);
	label_wait_rdgot:
	while ((n = r->rd_got))
		yaw_inline_futex_wait(&r->rd_got, n, NULL);
	while (r->rd_wait != r->rd_all)
	{
		if (r->rd_got)
			goto label_wait_rdgot;
	}
}

static inline void yaw_rwlock_rd_unlock(yaw_rwlock_s *r)
{
	uint32_t got_number;
	got_number = __sync_sub_and_fetch(&r->rd_got, 1);
	__sync_fetch_and_sub(&r->rd_all, 1);
	if (!got_number && r->wr_request)
		yaw_inline_futex_wake(&r->rd_got, 1);
}

static inline void yaw_rwlock_wr_unlock(yaw_rwlock_s *r)
{
	__sync_lock_test_and_set(&r->wr_mutex, 0);
	yaw_inline_futex_wake(&r->wr_mutex, 1);
	if (!__sync_sub_and_fetch(&r->wr_request, 1) && r->rd_all)
		yaw_inline_futex_wake(&r->wr_request, INT32_MAX);
}

static inline yaw_rwlock_s* yaw_rwlock_rd_try(yaw_rwlock_s *r)
{
	if (!r->wr_request)
	{
		__sync_fetch_and_add(&r->rd_all, 1);
		if (!r->wr_request)
		{
			__sync_fetch_and_add(&r->rd_got, 1);
			return r;
		}
		__sync_fetch_and_sub(&r->rd_all, 1);
	}
	return NULL;
}

static inline yaw_rwlock_s* yaw_rwlock_wr_try(yaw_rwlock_s *r)
{
	if (!r->rd_all && !r->wr_request)
	{
		__sync_fetch_and_add(&r->wr_request, 1);
		if (!__sync_val_compare_and_swap(&r->wr_mutex, 0, 1))
		{
			if (!r->rd_got)
			{
				while (r->rd_wait != r->rd_all)
				{
					if (r->rd_got)
						goto label_fail;
				}
				return r;
			}
			label_fail:
			__sync_lock_test_and_set(&r->wr_mutex, 0);
			yaw_inline_futex_wake(&r->wr_mutex, 1);
		}
		if (!__sync_sub_and_fetch(&r->wr_request, 1) && r->rd_all)
			yaw_inline_futex_wake(&r->wr_request, INT32_MAX);
	}
	return NULL;
}

static inline yaw_rwlock_s* yaw_rwlock_rd_wait(yaw_rwlock_s *r, uintptr_t usec)
{
	struct timespec c, d, t;
	uint32_t n;
	__sync_fetch_and_add(&r->rd_all, 1);
	if (!yaw_inline_timespec_curr(&c))
		goto label_fail;
	yaw_inline_timespec_init(&d, &c, usec);
	while ((n = r->wr_request))
	{
		if (!yaw_inline_timespec_curr(&c) || !yaw_inline_timespec_calc(&t, &c, &d))
			goto label_fail;
		__sync_fetch_and_add(&r->rd_wait, 1);
		yaw_inline_futex_wait(&r->wr_request, n, NULL);
		__sync_fetch_and_sub(&r->rd_wait, 1);
	}
	__sync_fetch_and_add(&r->rd_got, 1);
	return r;
	label_fail:
	__sync_fetch_and_sub(&r->rd_all, 1);
	return NULL;
}

static inline yaw_rwlock_s* yaw_rwlock_wr_wait(yaw_rwlock_s *r, uintptr_t usec)
{
	struct timespec c, d, t;
	uint32_t n;
	__sync_fetch_and_add(&r->wr_request, 1);
	if (!yaw_inline_timespec_curr(&c))
		goto label_fail_without_mutex;
	yaw_inline_timespec_init(&d, &c, usec);
	while (__sync_val_compare_and_swap(&r->wr_mutex, 0, 1))
	{
		if (!yaw_inline_timespec_curr(&c) || !yaw_inline_timespec_calc(&t, &c, &d))
			goto label_fail_without_mutex;
		yaw_inline_futex_wait(&r->wr_mutex, 1, NULL);
	}
	label_wait_rdgot:
	while ((n = r->rd_got))
	{
		if (!yaw_inline_timespec_curr(&c) || !yaw_inline_timespec_calc(&t, &c, &d))
			goto label_fail_with_mutex;
		yaw_inline_futex_wait(&r->rd_got, n, NULL);
	}
	while (r->rd_wait != r->rd_all)
	{
		if (r->rd_got)
			goto label_wait_rdgot;
	}
	return r;
	label_fail_with_mutex:
	__sync_lock_test_and_set(&r->wr_mutex, 0);
	yaw_inline_futex_wake(&r->wr_mutex, 1);
	label_fail_without_mutex:
	if (!__sync_sub_and_fetch(&r->wr_request, 1) && r->rd_all)
		yaw_inline_futex_wake(&r->wr_request, INT32_MAX);
	return NULL;
}

#define me(_r)  ((yaw_lock_rwlock_s *) _r)

static void yaw_lock_rwlock_free(yaw_lock_rwlock_s *restrict r)
{
	if (r->rwlock)
		refer_free(r->rwlock);
}

static void yaw_lock_rwlock_lock_read(yaw_lock_s *r)
{
	yaw_rwlock_rd_lock(me(r)->rwlock);
}

static void yaw_lock_rwlock_lock_write(yaw_lock_s *r)
{
	yaw_rwlock_wr_lock(me(r)->rwlock);
}

static void yaw_lock_rwlock_unlock_read(yaw_lock_s *r)
{
	yaw_rwlock_rd_unlock(me(r)->rwlock);
}

static void yaw_lock_rwlock_unlock_write(yaw_lock_s *r)
{
	yaw_rwlock_wr_unlock(me(r)->rwlock);
}

static yaw_lock_s* yaw_lock_rwlock_trylock_read(yaw_lock_s *r)
{
	if (yaw_rwlock_rd_try(me(r)->rwlock))
		return r;
	return NULL;
}

static yaw_lock_s* yaw_lock_rwlock_trylock_write(yaw_lock_s *r)
{
	if (yaw_rwlock_wr_try(me(r)->rwlock))
		return r;
	return NULL;
}

static yaw_lock_s* yaw_lock_rwlock_waitlock_read(yaw_lock_s *r, uintptr_t usec)
{
	if (yaw_rwlock_rd_wait(me(r)->rwlock, usec))
		return r;
	return NULL;
}

static yaw_lock_s* yaw_lock_rwlock_waitlock_write(yaw_lock_s *r, uintptr_t usec)
{
	if (yaw_rwlock_wr_wait(me(r)->rwlock, usec))
		return r;
	return NULL;
}

int yaw_lock_alloc_rwlock(yaw_lock_s *restrict *restrict read, yaw_lock_s *restrict *restrict write)
{
	yaw_rwlock_s *rwlock;
	yaw_lock_rwlock_s *restrict r, *restrict w;
	int ret;
	r = w = NULL;
	ret = -1;
	if (read && write)
	{
		if ((rwlock = (yaw_rwlock_s *) refer_alloz(sizeof(yaw_rwlock_s))))
		{
			r = (yaw_lock_rwlock_s *) refer_alloc(sizeof(yaw_lock_rwlock_s));
			w = (yaw_lock_rwlock_s *) refer_alloc(sizeof(yaw_lock_rwlock_s));
			if (r && w)
			{
				r->rwlock = (yaw_rwlock_s *) refer_save(rwlock);
				w->rwlock = (yaw_rwlock_s *) refer_save(rwlock);
				refer_set_free(r, (refer_free_f) yaw_lock_rwlock_free);
				refer_set_free(w, (refer_free_f) yaw_lock_rwlock_free);
				// read
				r->ym.lock = yaw_lock_rwlock_lock_read;
				r->ym.unlock = yaw_lock_rwlock_unlock_read;
				r->ym.trylock = yaw_lock_rwlock_trylock_read;
				r->ym.waitlock = yaw_lock_rwlock_waitlock_read;
				// write
				w->ym.lock = yaw_lock_rwlock_lock_write;
				w->ym.unlock = yaw_lock_rwlock_unlock_write;
				w->ym.trylock = yaw_lock_rwlock_trylock_write;
				w->ym.waitlock = yaw_lock_rwlock_waitlock_write;
				// ret
				ret = 0;
			}
			else
			{
				if (r) refer_free(r);
				if (w) refer_free(w);
				r = w = NULL;
			}
			refer_free(rwlock);
		}
	}
	if (read) *read = &r->ym;
	if (write) *write = &w->ym;
	return ret;
}
