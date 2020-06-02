#define _DEFAULT_SOURCE
#include "mlog.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

static void mlog_free_func(mlog_s *restrict r)
{
	if (r->mlog) free(r->mlog);
}

mlog_s* mlog_alloc(uint32_t init2exp)
{
	mlog_s *restrict r;
	if (!init2exp) init2exp = 14;
	if (init2exp < 32)
	{
		r = (mlog_s *) refer_alloc(sizeof(mlog_s));
		if (r)
		{
			r->size = 1 << init2exp;
			r->mlog = (char *) malloc(r->size);
			if (r->mlog)
			{
				r->length = 0;
				*r->mlog = 0;
				refer_set_free(r, (refer_free_f) mlog_free_func);
				return r;
			}
			refer_free(r);
		}
	}
	return NULL;
}

mlog_s* mlog_expand(mlog_s *restrict r)
{
	char *rr;
	size_t n;
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

mlog_s* mlog_printf(mlog_s *restrict r, const char *restrict fmt, ...)
{
	va_list list;
	size_t n, nn;
	if (r->length + 1 < r->size || (r = mlog_expand(r)))
	{
		label_try:
		va_start(list, fmt);
		nn = r->size - r->length;
		if ((int) (n = vsnprintf(r->mlog + r->length, nn, fmt, list)) < 0)
			r = NULL;
		else if (n + 1 >= nn && (r = mlog_expand(r)))
		{
			va_end(list);
			goto label_try;
		}
		va_end(list);
		r->length += n;
	}
	return r;
}

