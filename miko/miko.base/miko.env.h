#ifndef _miko_base_miko_env_h_
#define _miko_base_miko_env_h_

#include "miko.h"
#include <vattr.h>

struct miko_env_s {
	mlog_s *mlog;
	miko_log_s *log;
	miko_iset_pool_s *pool;
	const miko_env_runtime_s *runtime;
	// iset-merge
	vattr_s *segment;
	vattr_s *action;
	vattr_s *interrupt;
	vattr_s *major;
	vattr_s *process;
	vattr_s *instruction;
};

const miko_env_runtime_s* miko_env_runtime_alloc(const miko_env_s *restrict env);
miko_env_s* miko_env_okay(miko_env_s *restrict r);

#endif
