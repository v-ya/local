#ifndef _miko_h_
#define _miko_h_

#include <refer.h>
#include <mlog.h>
#include "miko.type.env.h"
#include "miko.type.rt.h"

miko_env_s* miko_env_create(mlog_s *restrict mlog, miko_env_register_f register_array[]);

#endif
