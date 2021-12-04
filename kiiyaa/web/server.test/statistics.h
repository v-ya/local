#ifndef _statistics_h_
#define _statistics_h_

#include <rbtree.h>
#include <vattr.h>
#include <yaw.h>
#include <mlog.h>

typedef struct statistics_s {
	vattr_s *st;
	yaw_lock_s *r;
	yaw_lock_s *w;
} statistics_s;

typedef struct statistics_item_s {
	refer_nstring_t uri;
	volatile uintptr_t count;
} statistics_item_s;

extern statistics_s *statistics;

statistics_s* statistics_alloc(void);
statistics_s* statistics_touch(statistics_s *restrict r, refer_nstring_t uri);
void statistics_dump(statistics_s *restrict r, mlog_s *mlog);

#endif
