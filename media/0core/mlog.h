#ifndef _media_core_mlog_h_
#define _media_core_mlog_h_

#include <refer.h>
#include <mlog.h>

struct media_mlog_data_s {
	mlog_s *mlog;
	refer_string_t loglevel;
	uint64_t tsms_start;
};

mlog_s* media_mlog_alloc(mlog_s *restrict mlog, const char *restrict loglevel, uint64_t tsms_start, struct yaw_lock_s *restrict lock);

#endif
