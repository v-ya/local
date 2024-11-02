#ifndef _miko_base_miko_rt_h_
#define _miko_base_miko_rt_h_

#include "miko.h"

struct miko_rt_backup_s {
	const miko_rt_s *runtime;
	const miko_table_s *process_backup;
	miko_rt_dynamic_t dynamic_backup;
};

const miko_rt_backup_s* miko_rt_backup_alloc(const miko_rt_s *restrict runtime);
miko_rt_s* miko_rt_backup_goback(const miko_rt_backup_s *restrict r, miko_rt_s *restrict runtime);

#endif
