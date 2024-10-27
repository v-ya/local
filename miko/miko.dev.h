#ifndef _miko_dev_h_
#define _miko_dev_h_

#include "miko.h"
#include "miko.type.h"

miko_iset_s* miko_iset_alloc(const char *restrict name);
miko_iset_s* miko_iset_add_depend(miko_iset_s *restrict r, const char *restrict name);
miko_iset_s* miko_iset_add_type(miko_iset_s *restrict r, const char *restrict name);
miko_iset_s* miko_iset_add_instruction(miko_iset_s *restrict r, const miko_iset_pool_s *restrict pool, const char *restrict name, const miko_instruction_argv_t argv[], const miko_instruction_attr_t attr[], refer_t pri);

#endif
