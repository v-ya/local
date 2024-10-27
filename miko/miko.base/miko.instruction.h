#ifndef _miko_base_miko_instruction_h_
#define _miko_base_miko_instruction_h_

#include "miko.h"

struct miko_instruction_func_t {
	miko_instruction_attr_call_f call;
};

struct miko_instruction_s {
	refer_string_t name;
	refer_string_t set;
	refer_t pri;
	miko_instruction_args_s *args;
	miko_instruction_func_t func;
};

miko_instruction_s* miko_instruction_alloc(const char *restrict name, refer_string_t set, refer_t pri);
miko_instruction_s* miko_instruction_set_args(miko_instruction_s *restrict r, const miko_iset_pool_s *restrict pool, const miko_instruction_argv_t argv[]);
miko_instruction_s* miko_instruction_set_func(miko_instruction_s *restrict r, const miko_instruction_attr_t attr[]);

#endif
