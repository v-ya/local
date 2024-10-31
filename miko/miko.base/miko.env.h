#ifndef _miko_base_miko_env_h_
#define _miko_base_miko_env_h_

#include "miko.h"
#include <vattr.h>

struct miko_env_s {
	mlog_s *mlog;
	miko_log_s *log;
	miko_iset_pool_s *pool;
	// iset-merge
	vattr_s *segment;
	vattr_s *action;
	vattr_s *major;
	vattr_s *instruction;
};

miko_env_s* miko_env_okay(miko_env_s *restrict r);

#endif
