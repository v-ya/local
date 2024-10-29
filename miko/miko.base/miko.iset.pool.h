#ifndef _miko_base_miko_iset_pool_h_
#define _miko_base_miko_iset_pool_h_

#include "miko.h"

// miko_iset_pool_s: miko.iset.name => (miko_iset_s) miko.iset

const miko_iset_pool_s* miko_iset_pool_save_depend(const miko_iset_pool_s *restrict pool, const miko_iset_s *restrict iset, miko_log_s *restrict log);
refer_string_t miko_iset_pool_save_type(const miko_iset_pool_s *restrict pool, const char *restrict name);

#endif
