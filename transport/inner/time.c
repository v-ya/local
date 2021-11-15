#define _DEFAULT_SOURCE
#include "time.h"

int transport_inner_sleep_maybe_ms(uint64_t ms)
{
	struct timespec t;
	t.tv_sec = ms / 1000;
	t.tv_nsec = (ms % 1000) * 1000000;
	return nanosleep(&t, NULL);
}

struct timespec* transport_inner_timespec_now(struct timespec *restrict r)
{
	if (!clock_gettime(CLOCK_MONOTONIC_COARSE, r))
		return r;
	return NULL;
}

struct timespec* transport_inner_timespec_diff(struct timespec *r, const struct timespec *a, const struct timespec *b)
{
	if ((a->tv_nsec >= b->tv_nsec))
	{
		r->tv_sec = a->tv_sec - b->tv_sec;
		r->tv_nsec = a->tv_nsec - b->tv_nsec;
	}
	else
	{
		r->tv_sec = a->tv_sec - b->tv_sec - 1;
		r->tv_nsec = a->tv_nsec + 1000000000 - b->tv_nsec;
	}
	return r;
}

struct timespec* transport_inner_timespec_by_ms(struct timespec *restrict r, uint64_t ms)
{
	r->tv_sec = ms / 1000;
	r->tv_nsec = (ms % 1000) * 1000000;
	return r;
}

struct timespec* transport_inner_timespec_by_us(struct timespec *restrict r, uint64_t us)
{
	r->tv_sec = us / 1000000;
	r->tv_nsec = (us % 1000000) * 1000;
	return r;
}

uint64_t transport_inner_timespec_to_ms(struct timespec *restrict r)
{
	return (uint64_t) r->tv_sec * 1000 + (uint64_t) r->tv_nsec / 1000000;
}

uint64_t transport_inner_timespec_to_us(struct timespec *restrict r)
{
	return (uint64_t) r->tv_sec * 1000000 + (uint64_t) r->tv_nsec / 1000;
}

struct timeval* transport_inner_timeval_by_ms(struct timeval *restrict r, uint64_t ms)
{
	r->tv_sec = ms / 1000;
	r->tv_usec = (ms % 1000) * 1000;
	return r;
}

struct timeval* transport_inner_timeval_by_us(struct timeval *restrict r, uint64_t us)
{
	r->tv_sec = us / 1000000;
	r->tv_usec = us % 1000000;
	return r;
}

uint64_t transport_inner_timeval_to_ms(struct timeval *restrict r)
{
	return (uint64_t) r->tv_sec * 1000 + (uint64_t) r->tv_usec / 1000;
}

uint64_t transport_inner_timeval_to_us(struct timeval *restrict r)
{
	return (uint64_t) r->tv_sec * 1000000 + (uint64_t) r->tv_usec;
}
