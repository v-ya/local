#ifndef _layer_log_h_
#define _layer_log_h_

#include <refer.h>
#include <mlog.h>

struct yaw_lock_s;

typedef struct layer_log_s layer_log_s;

struct layer_log_s {
	mlog_s *input;
};

layer_log_s* layer_log_alloc(mlog_s *restrict output, struct yaw_lock_s *restrict locker);
layer_log_s* layer_log_nspace(layer_log_s *restrict log, uint32_t nspace);
layer_log_s* layer_log_push(layer_log_s *restrict log, uint32_t nindex);
layer_log_s* layer_log_pop(layer_log_s *restrict log, uint32_t nindex);
void layer_log_reset(layer_log_s *restrict log);

#endif
