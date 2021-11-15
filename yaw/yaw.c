#define _DEFAULT_SOURCE
#include "yaw.h"
#include <pthread.h>
#include <time.h>
#include <errno.h>

typedef struct yaw_real_s {
	yaw_s yaw;
	pthread_t thread;
	uintptr_t detach;
} yaw_real_s;

static void yaw_real_free_func(yaw_real_s *restrict r)
{
	r->yaw.running = 0;
	if (r->thread && !r->detach)
	{
		pthread_t thread;
		if ((thread = pthread_self()) != r->thread)
			pthread_join(r->thread, NULL);
		else pthread_detach(thread);
	}
	if (r->yaw.pri)
		refer_free(r->yaw.pri);
}

static void* yaw_thread_func(void *yaw)
{
	((yaw_s *) yaw)->func((yaw_s *) yaw);
	((yaw_s *) yaw)->running = 0;
	refer_free(yaw);
	return NULL;
}

yaw_s* yaw_alloc(yaw_do_f func, refer_t pri)
{
	yaw_real_s *restrict r;
	r = (yaw_real_s *) refer_alloz(sizeof(yaw_real_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) yaw_real_free_func);
		r->yaw.func = func;
		r->yaw.pri = refer_save(pri);
	}
	return &r->yaw;
}

yaw_s* yaw_start(yaw_s *restrict yaw, const void *data)
{
	if (!((yaw_real_s *) yaw)->thread)
	{
		yaw->running = 1;
		yaw->data = (void *) data;
		((yaw_real_s *) yaw)->detach = 0;
		refer_save(yaw);
		if (!pthread_create(&((yaw_real_s *) yaw)->thread, NULL, yaw_thread_func, yaw))
			return yaw;
		refer_free(yaw);
	}
	return NULL;
}

yaw_s* yaw_alloc_and_start(yaw_do_f func, refer_t pri, const void *data)
{
	yaw_s *restrict r;
	if ((r = yaw_alloc(func, pri)))
	{
		if (yaw_start(r, data))
			return r;
		refer_free(r);
	}
	return NULL;
}

void yaw_stop(yaw_s *restrict yaw)
{
	((yaw_s *) yaw)->running = 0;
}

yaw_s* yaw_wait(yaw_s *restrict yaw)
{
	pthread_t thread;
	if ((thread = ((yaw_real_s *) yaw)->thread) && pthread_self() != thread)
	{
		if (!((yaw_real_s *) yaw)->detach && !pthread_join(thread, NULL))
		{
			((yaw_real_s *) yaw)->detach = 1;
			return yaw;
		}
	}
	return NULL;
}

yaw_s* yaw_stop_and_wait(yaw_s *restrict yaw)
{
	yaw_stop(yaw);
	return yaw_wait(yaw);
}

yaw_s* yaw_detach_self(yaw_s *restrict yaw)
{
	pthread_t thread;
	if ((thread = ((yaw_real_s *) yaw)->thread) && pthread_self() == thread)
	{
		if (!pthread_detach(thread))
		{
			((yaw_real_s *) yaw)->detach = 1;
			return yaw;
		}
	}
	return NULL;
}

static void yaw_sleep_tspec(uintptr_t sec, uintptr_t nsec)
{
	struct timespec ts;
	ts.tv_sec = sec;
	ts.tv_nsec = nsec;
	while (nanosleep(&ts, &ts) && errno == EINTR) ;
}

void yaw_sleep(uintptr_t sec)
{
	yaw_sleep_tspec(sec, 0);
}

void yaw_msleep(uintptr_t msec)
{
	yaw_sleep_tspec(msec / 1000, (msec % 1000) * 1000000);
}

void yaw_usleep(uintptr_t usec)
{
	yaw_sleep_tspec(usec / 1000000, (usec % 1000000) * 1000);
}

void yaw_nsleep(uintptr_t nsec)
{
	yaw_sleep_tspec(nsec / 1000000000, nsec % 1000000000);
}

#include "inline_futex.h"

uint64_t yaw_timestamp_sec(void)
{
	struct timespec ts;
	if (yaw_inline_timespec_curr(&ts))
		return (uint64_t) ts.tv_sec;
	return 0;
}

uint64_t yaw_timestamp_msec(void)
{
	struct timespec ts;
	if (yaw_inline_timespec_curr(&ts))
		return (uint64_t) ts.tv_sec * dt_s2ms + (uint64_t) ts.tv_nsec / dt_ms2ns;
	return 0;
}

uint64_t yaw_timestamp_usec(void)
{
	struct timespec ts;
	if (yaw_inline_timespec_curr(&ts))
		return (uint64_t) ts.tv_sec * dt_s2us + (uint64_t) ts.tv_nsec / dt_us2ns;
	return 0;
}

uint64_t yaw_timestamp_nsec(void)
{
	struct timespec ts;
	if (yaw_inline_timespec_curr(&ts))
		return (uint64_t) ts.tv_sec * dt_s2ns + (uint64_t) ts.tv_nsec;
	return 0;
}
