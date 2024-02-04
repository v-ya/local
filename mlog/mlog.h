#ifndef _mlog_h_
#define _mlog_h_

#include <refer.h>

struct yaw_lock_s;

typedef struct mlog_s mlog_s;
typedef int (*mlog_report_f)(const char *restrict msg, uintptr_t length, refer_t pri);
typedef mlog_s* (*mlog_wdone_f)(mlog_s *restrict mlog, void *restrict pri, uintptr_t *restrict rlength);

struct mlog_s {
	char *mlog;
	uintptr_t size;
	uintptr_t length;
	mlog_report_f report;
	refer_t pri;
	struct yaw_lock_s *locker;
};

// locker used only by: mlog_wdone, mlog_write, mlog_printf

mlog_s* mlog_alloc(uint32_t init2exp);
mlog_s* mlog_set_report(mlog_s *restrict r, mlog_report_f report_func, refer_t pri);
mlog_s* mlog_set_locker(mlog_s *restrict r, struct yaw_lock_s *locker);
mlog_s* mlog_expand(mlog_s *restrict r);
char* mlog_wneed(mlog_s *restrict r, uintptr_t size);
mlog_s* mlog_wdone(mlog_s *restrict r, mlog_wdone_f wdone_func, const void *restrict wdone_pri);
mlog_s* mlog_write(mlog_s *restrict r, const char *restrict data, uintptr_t size);
mlog_s* mlog_printf(mlog_s *restrict r, const char *restrict fmt, ...) __attribute__ ((__format__ (__printf__, 2, 0)));
mlog_s* mlog_clear(mlog_s *restrict r);

int mlog_report_stdout_func(const char *restrict msg, uintptr_t length, refer_t pri);

#endif
