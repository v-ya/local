#ifndef _miko_type_process_h_
#define _miko_type_process_h_

#include <refer.h>
#include "miko.type.env.runtime.h"

typedef struct miko_process_s miko_process_s;
typedef struct miko_process_runtime_s miko_process_runtime_s;

typedef miko_process_s* (*miko_process_initial_f)(miko_process_s *restrict r, const miko_env_runtime_s *restrict env);
typedef miko_process_runtime_s* (*miko_process_create_f)(const miko_process_s *restrict r);
typedef void (*miko_process_runtime_reset_f)(miko_process_runtime_s *restrict r);
typedef refer_t (*miko_process_runtime_backup_f)(miko_process_runtime_s *restrict r);
typedef miko_process_runtime_s* (*miko_process_runtime_goback_f)(miko_process_runtime_s *restrict r, refer_t backup);

struct miko_process_s {
	miko_process_initial_f initial;
	miko_process_create_f create;
};

struct miko_process_runtime_s {
	miko_process_runtime_reset_f reset;
	miko_process_runtime_backup_f backup;
	miko_process_runtime_goback_f goback;
};

#endif
