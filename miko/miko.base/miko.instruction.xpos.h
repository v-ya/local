#ifndef _miko_base_miko_instruction_xpos_h_
#define _miko_base_miko_instruction_xpos_h_

#include "miko.h"

struct miko_instruction_xpos_t {
	refer_string_t score;
	refer_string_t style;
	refer_string_t major;
	refer_string_t minor;
};

struct miko_instruction_xpos_s {
	uintptr_t count;
	miko_instruction_xpos_t xpos[];
};

miko_instruction_xpos_s* miko_instruction_xpos_alloc(const miko_iset_pool_s *restrict pool, const miko_xpos_prefix_t xpos[]);

#endif
