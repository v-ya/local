#ifndef _miko_base_miko_env_h_
#define _miko_base_miko_env_h_

#include "miko.h"
#include <vattr.h>

struct miko_env_s {
	mlog_s *mlog;
	miko_iset_pool_s *pool;
};

miko_env_s* miko_env_alloc(mlog_s *restrict mlog);

#endif
