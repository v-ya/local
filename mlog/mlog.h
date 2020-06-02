#ifndef _mlog_h_
#define _mlog_h_

#include <refer.h>

typedef struct mlog_s {
	char *mlog;
	size_t size;
	size_t length;
} mlog_s;

mlog_s* mlog_alloc(uint32_t init2exp);
mlog_s* mlog_expand(mlog_s *restrict r);
mlog_s* mlog_printf(mlog_s *restrict r, const char *restrict fmt, ...) __attribute__ ((__format__ (__printf__, 2, 0)));

#endif
