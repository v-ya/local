#ifndef _mlog_h_
#define _mlog_h_

#include <refer.h>

typedef int (*mlog_report_f)(const char *restrict msg, size_t length, refer_t pri);
typedef struct mlog_s {
	char *mlog;
	size_t size;
	size_t length;
	mlog_report_f report;
	refer_t pri;
} mlog_s;

mlog_s* mlog_alloc(uint32_t init2exp);
mlog_s* mlog_set_report(mlog_s *restrict r, mlog_report_f report_func, refer_t pri);
mlog_s* mlog_expand(mlog_s *restrict r);
mlog_s* mlog_printf(mlog_s *restrict r, const char *restrict fmt, ...) __attribute__ ((__format__ (__printf__, 2, 0)));
mlog_s* mlog_clear(mlog_s *restrict r);

#endif
