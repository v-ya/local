#ifndef _miko_base_miko_instruction_args_h_
#define _miko_base_miko_instruction_args_h_

#include "miko.h"

struct miko_instruction_args_t {
	refer_string_t xfind_type;
	refer_string_t value_type;
};

struct miko_instruction_args_s {
	uintptr_t argc;
	miko_instruction_args_t argv[];
};

miko_instruction_args_s* miko_instruction_args_alloc(const miko_iset_pool_s *restrict pool, const miko_instruction_argv_t argv[]);

#endif
