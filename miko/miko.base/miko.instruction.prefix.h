#ifndef _miko_base_miko_instruction_prefix_h_
#define _miko_base_miko_instruction_prefix_h_

#include "miko.h"

struct miko_instruction_prefix_t {
	refer_string_t segment;
	refer_string_t action;
	refer_string_t major;
	refer_string_t minor;
};

struct miko_instruction_prefix_s {
	miko_count_t count;
	miko_instruction_prefix_t prefix[];
};

miko_instruction_prefix_s* miko_instruction_prefix_alloc(const miko_iset_pool_s *restrict pool, const miko_access_prefix_t prefix[]);

#endif
