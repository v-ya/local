#ifndef _miko_base_miko_instruction_h_
#define _miko_base_miko_instruction_h_

#include "miko.h"
#include "miko.instruction.prefix.h"

typedef struct miko_instruction_func_t miko_instruction_func_t;

struct miko_instruction_func_t {
	miko_instruction_attr_call_f call;
};

struct miko_instruction_s {
	refer_string_t name;
	refer_string_t iset;
	refer_t data;
	miko_instruction_prefix_s *prefix;
	miko_instruction_func_t func;
};

miko_instruction_s* miko_instruction_alloc(const char *restrict name, refer_string_t iset);
miko_instruction_s* miko_instruction_set_prefix(miko_instruction_s *restrict r, const miko_iset_pool_s *restrict pool, const miko_access_prefix_t prefix[]);
miko_instruction_s* miko_instruction_set_attr(miko_instruction_s *restrict r, const miko_instruction_attr_t attr[]);

#endif
