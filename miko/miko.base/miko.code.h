#ifndef _miko_base_miko_code_h_
#define _miko_base_miko_code_h_

#include "miko.h"
#include <exbuffer.h>

typedef struct miko_code_impl_s miko_code_impl_s;

struct miko_code_builder_s {
	const miko_env_runtime_s *runtime;
	miko_code_impl_s *cache;
	exbuffer_t prefix;
	exbuffer_t access;
};

struct miko_code_impl_s {
	miko_code_s code;
	exbuffer_t instr;
	exbuffer_t ipos;
	exbuffer_t array;
};

miko_code_impl_s* miko_code_alloc(void);
miko_code_impl_s* miko_code_push_instruction(miko_code_impl_s *restrict r, const miko_env_runtime_s *restrict runtime, miko_index_t instruction, const miko_access_t *restrict access);
miko_code_impl_s* miko_code_push_code(miko_code_impl_s *restrict r, const miko_code_s *restrict code);
miko_code_impl_s* miko_code_pop(miko_code_impl_s *restrict r, miko_count_t count);
miko_instr_t* miko_code_get_by_index(const miko_code_impl_s *restrict r, miko_offset_t index);
miko_code_s* miko_code_okay(miko_code_impl_s *restrict r);

#endif
