#ifndef _yaw_inline_futex_h_
#define _yaw_inline_futex_h_

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#include <stdint.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h>

#define dt_us2ns 1000
#define dt_s2us  1000000
#define dt_s2ns  1000000000

static inline struct timespec* yaw_inline_timespec_curr(struct timespec *__restrict c)
{
	if (!clock_gettime(CLOCK_MONOTONIC_COARSE, c))
		return c;
	return NULL;
}

static inline struct timespec* yaw_inline_timespec_init(struct timespec *__restrict d, const struct timespec *__restrict c, uintptr_t usec)
{
	d->tv_sec = c->tv_sec + (usec / dt_s2us);
	d->tv_nsec = c->tv_nsec + ((usec % dt_s2us) * dt_us2ns);
	if (d->tv_nsec >= dt_s2ns)
	{
		d->tv_sec += 1;
		d->tv_nsec -= dt_s2ns;
	}
	return d;
}

static inline struct timespec* yaw_inline_timespec_calc(struct timespec *__restrict t, const struct timespec *__restrict c, const struct timespec *__restrict d)
{
	if (c->tv_sec < d->tv_sec || (c->tv_sec == d->tv_sec && c->tv_nsec < d->tv_nsec))
	{
		t->tv_sec = d->tv_sec - c->tv_sec;
		if (d->tv_nsec >= c->tv_nsec)
			t->tv_nsec = d->tv_nsec - c->tv_nsec;
		else
		{
			t->tv_nsec = dt_s2ns + d->tv_nsec - c->tv_nsec;
			t->tv_sec -= 1;
		}
		return t;
	}
	return NULL;
}

#undef dt_us2ns
#undef dt_s2us
#undef dt_s2ns

typedef uint32_t (*yaw_inline_futex_wait_test_f)(volatile uint32_t *address, uint32_t waitat);

static inline uint32_t yaw_inline_futex_wake(volatile uint32_t *address, uint32_t number)
{
	return syscall(SYS_futex, address, FUTEX_WAKE_PRIVATE, number);
}

static inline void yaw_inline_futex_wait(volatile uint32_t *address, uint32_t waitat, const struct timespec *ts)
{
	syscall(SYS_futex, address, FUTEX_WAIT_PRIVATE, waitat, ts);
}

static inline void yaw_inline_futex_wait_okay(volatile uint32_t *address, uint32_t waitat, yaw_inline_futex_wait_test_f test)
{
	while (test(address, waitat))
		syscall(SYS_futex, address, FUTEX_WAIT_PRIVATE, waitat, NULL);
}

static inline volatile uint32_t* yaw_inline_futex_wait_time(volatile uint32_t *address, uint32_t waitat, yaw_inline_futex_wait_test_f test, uintptr_t usec)
{
	if (test(address, waitat))
	{
		struct timespec c, d, t;
		if (!yaw_inline_timespec_curr(&c))
			goto label_fail;
		yaw_inline_timespec_init(&d, &c, usec);
		goto label_loop;
		do {
			if (!yaw_inline_timespec_curr(&c))
				goto label_fail;
			label_loop:
			if (!yaw_inline_timespec_calc(&t, &c, &d))
				goto label_fail;
			syscall(SYS_futex, address, FUTEX_WAIT_PRIVATE, waitat, &t);
		} while (test(address, waitat));
	}
	return address;
	label_fail:
	return NULL;
}

#endif
