#define _DEFAULT_SOURCE
#include "mlog.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <memory.h>
#include <yaw.h>

typedef struct mlog_write_t mlog_write_t;
typedef struct mlog_printf_t mlog_printf_t;

struct mlog_write_t {
	const char *data;
	uintptr_t size;
};
struct mlog_printf_t {
	const char *format;
	va_list list;
};

static void mlog_free_func(mlog_s *restrict r)
{
	if (r->mlog) free(r->mlog);
	if (r->pri) refer_free(r->pri);
	if (r->locker) refer_free(r->locker);
}

mlog_s* mlog_alloc(uint32_t init2exp)
{
	mlog_s *restrict r;
	if (!init2exp) init2exp = 14;
	if (init2exp < 32)
	{
		r = (mlog_s *) refer_alloz(sizeof(mlog_s));
		if (r)
		{
			r->size = 1 << init2exp;
			r->mlog = (char *) malloc(r->size);
			if (r->mlog)
			{
				*r->mlog = 0;
				refer_set_free(r, (refer_free_f) mlog_free_func);
				return r;
			}
			refer_free(r);
		}
	}
	return NULL;
}

mlog_s* mlog_set_report(mlog_s *restrict r, mlog_report_f report_func, refer_t pri)
{
	if (r->pri) refer_free(r->pri);
	r->report = report_func;
	r->pri = refer_save(pri);
	return r;
}

mlog_s* mlog_set_locker(mlog_s *restrict r, struct yaw_lock_s *locker)
{
	if (r->locker)
		refer_free(r->locker);
	r->locker = (struct yaw_lock_s *) refer_save(locker);
	return r;
}

mlog_s* mlog_expand(mlog_s *restrict r)
{
	char *rr;
	uintptr_t n;
	n = r->size << 1;
	if (n)
	{
		rr = realloc(r->mlog, n);
		if (rr)
		{
			r->mlog = rr;
			r->size = n;
			return r;
		}
	}
	return NULL;
}

char* mlog_wneed(mlog_s *restrict r, uintptr_t size)
{
	size += r->length;
	while (size > r->size)
	{
		if (!(r = mlog_expand(r)))
			goto label_fail;
	}
	return r->mlog + r->length;
	label_fail:
	return NULL;
}

mlog_s* mlog_wdone(mlog_s *restrict r, mlog_wdone_f wdone_func, const void *restrict wdone_pri)
{
	yaw_lock_s *restrict locker;
	uintptr_t n, pos;
	if (r && wdone_func)
	{
		if ((locker = r->locker))
			yaw_lock_lock(locker);
		n = 0;
		if ((r = wdone_func(r, (void *) wdone_pri, &n)))
		{
			pos = n + r->length;
			if (pos < r->size || mlog_expand(r))
				r->mlog[pos] = 0;
			if (!r->report || !r->report(r->mlog + r->length, n, r->pri))
				r->length = pos;
		}
		if (locker)
			yaw_lock_unlock(locker);
	}
	return r;
}

static mlog_s* mlog_write_wdone(mlog_s *restrict mlog, const mlog_write_t *restrict pri, uintptr_t *restrict rlength)
{
	char *restrict p;
	if ((p = mlog_wneed(mlog, pri->size)))
	{
		memcpy(p, pri->data, *rlength = pri->size);
		return mlog;
	}
	return NULL;
}

mlog_s* mlog_write(mlog_s *restrict r, const char *restrict data, uintptr_t size)
{
	mlog_write_t pri;
	pri.data = data;
	pri.size = size;
	return mlog_wdone(r, (mlog_wdone_f) mlog_write_wdone, &pri);
}

static mlog_s* mlog_printf_wdone(mlog_s *restrict mlog, mlog_printf_t *restrict pri, uintptr_t *restrict rlength)
{
	uintptr_t n, nn;
	if (mlog->length + 1 < mlog->size || (mlog = mlog_expand(mlog)))
	{
		label_try:
		nn = mlog->size - mlog->length;
		if ((int) (n = vsnprintf(mlog->mlog + mlog->length, nn, pri->format, pri->list)) < 0)
			mlog = NULL;
		else if (n + 1 >= nn && (mlog = mlog_expand(mlog)))
			goto label_try;
		*rlength = n;
		return mlog;
	}
	return NULL;
}

mlog_s* mlog_printf(mlog_s *restrict r, const char *restrict fmt, ...)
{
	mlog_printf_t pri;
	pri.format = fmt;
	va_start(pri.list, fmt);
	r = mlog_wdone(r, (mlog_wdone_f) mlog_printf_wdone, &pri);
	va_end(pri.list);
	return r;
}

mlog_s* mlog_clear(mlog_s *restrict r)
{
	r->length = 0;
	*r->mlog = 0;
	return r;
}

// report func

int mlog_report_stdout_func(const char *restrict msg, uintptr_t length, refer_t pri)
{
	if (length) write(STDOUT_FILENO, msg, length);
	return 1;
}

