#ifndef _miko_h_
#define _miko_h_

#include <refer.h>
#include <mlog.h>

struct miko_log_s;
struct miko_iset_s;

typedef struct miko_env_s miko_env_s;
typedef struct miko_rt_s miko_rt_s;

typedef struct miko_iset_s* (*miko_env_register_f)(miko_env_s *restrict env, struct miko_log_s *restrict log);

miko_env_s* miko_env_create(mlog_s *restrict mlog, miko_env_register_f register_array[]);

#endif
