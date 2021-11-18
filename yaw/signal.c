#include "inline_futex.h"
#include "yaw.h"

typedef struct yaw_signal_futex_s {
	yaw_signal_s signal;
	volatile uint32_t status;
	volatile uint32_t number;
} yaw_signal_futex_s;

#define me(_r)  ((yaw_signal_futex_s *) _r)

static uint32_t yaw_signal_futex_set(yaw_signal_s *ys, uint32_t status)
{
	return (uint32_t) __sync_lock_test_and_set(&me(ys)->status, status);
}

static uint32_t yaw_signal_futex_get(yaw_signal_s *ys)
{
	return me(ys)->status;
}

static yaw_signal_s* yaw_signal_futex_wait(yaw_signal_s *ys, uint32_t status)
{
	if (me(ys)->status == status)
	{
		__sync_fetch_and_add(&me(ys)->number, 1);
		yaw_inline_futex_wait(&me(ys)->status, status, NULL);
		__sync_fetch_and_sub(&me(ys)->number, 1);
		return (me(ys)->status == status)?ys:NULL;
	}
	return NULL;
}

static uint32_t yaw_signal_futex_wake(yaw_signal_s *ys, uint32_t number)
{
	if (number > INT32_MAX)
		number = INT32_MAX;
	if (number)
		return yaw_inline_futex_wake(&me(ys)->status, number);
	return 0;
}

static yaw_signal_s* yaw_signal_futex_wait_time(yaw_signal_s *ys, uint32_t status, uintptr_t usec)
{
	if (me(ys)->status == status)
	{
		struct timespec ts = {
			.tv_sec = usec / 1000000,
			.tv_nsec = (usec % 1000000) * 1000
		};
		__sync_fetch_and_add(&me(ys)->number, 1);
		yaw_inline_futex_wait(&me(ys)->status, status, &ts);
		__sync_fetch_and_sub(&me(ys)->number, 1);
		return (me(ys)->status == status)?ys:NULL;
	}
	return NULL;
}

static uint32_t yaw_signal_futex_sink_number(yaw_signal_s *ys)
{
	return me(ys)->number;
}

static uint32_t yaw_signal_futex_inc(yaw_signal_s *ys)
{
	return (uint32_t) __sync_add_and_fetch(&me(ys)->status, 1);
}

yaw_signal_s* yaw_signal_alloc(void)
{
	yaw_signal_futex_s *restrict r;
	if ((r = (yaw_signal_futex_s *) refer_alloc(sizeof(yaw_signal_futex_s))))
	{
		r->signal.set = yaw_signal_futex_set;
		r->signal.get = yaw_signal_futex_get;
		r->signal.wait = yaw_signal_futex_wait;
		r->signal.wake = yaw_signal_futex_wake;
		r->signal.wait_time = yaw_signal_futex_wait_time;
		r->signal.sink_number = yaw_signal_futex_sink_number;
		r->signal.inc = yaw_signal_futex_inc;
		r->status = 0;
		r->number = 0;
	}
	return &r->signal;
}
