#ifndef _miko_api_iset_h_
#define _miko_api_iset_h_

#include "miko.type.iset.h"
#include "miko.type.major.h"
#include "miko.type.minor.h"
#include "miko.type.access.h"
#include "miko.type.instruction.h"

miko_iset_s* miko_iset_alloc(const char *restrict name);
miko_iset_s* miko_iset_add_depend(miko_iset_s *restrict r, const char *restrict name);
miko_iset_s* miko_iset_add_segment(miko_iset_s *restrict r, const char *restrict name);
miko_iset_s* miko_iset_add_action(miko_iset_s *restrict r, const char *restrict name);
miko_iset_s* miko_iset_add_major(miko_iset_s *restrict r, const char *restrict major_name, miko_major_vtype_t vtype);
miko_iset_s* miko_iset_add_minor(miko_iset_s *restrict r, const char *restrict major_name, miko_minor_s *restrict minor);
miko_iset_s* miko_iset_add_default_minor(miko_iset_s *restrict r, const char *restrict major_name, const char *restrict minor_name);
miko_iset_s* miko_iset_add_instruction(miko_iset_s *restrict r, const miko_iset_pool_s *restrict pool, const char *restrict name, const miko_access_prefix_t prefix[], const miko_instruction_attr_t attr[]);

#endif
