#ifndef _miko_type_env_h_
#define _miko_type_env_h_

#include <refer.h>

struct miko_log_s;
struct miko_iset_s;

typedef struct miko_env_s miko_env_s;

typedef struct miko_iset_s* (*miko_env_register_f)(miko_env_s *restrict env, struct miko_log_s *restrict log);

#endif
