#ifndef _gvcx_log_h_
#define _gvcx_log_h_

#include <refer.h>
#include <mlog.h>

struct yaw_lock_s;

typedef struct gvcx_log_s gvcx_log_s;

struct gvcx_log_s {
	mlog_s *input;
};

gvcx_log_s* gvcx_log_alloc(mlog_s *restrict output, struct yaw_lock_s *restrict locker);
gvcx_log_s* gvcx_log_nspace(gvcx_log_s *restrict log, uint32_t nspace);
gvcx_log_s* gvcx_log_push(gvcx_log_s *restrict log, uint32_t nindex);
gvcx_log_s* gvcx_log_pop(gvcx_log_s *restrict log, uint32_t nindex);
void gvcx_log_reset(gvcx_log_s *restrict log);

#endif
