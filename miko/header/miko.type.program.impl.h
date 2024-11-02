#ifndef _miko_type_program_impl_h_
#define _miko_type_program_impl_h_

#include <refer.h>
#include "miko.type.program.h"
#include "miko.type.program.argv.h"
#include "miko.type.rt.h"

typedef miko_rt_s* (*miko_program_symbol_f)(miko_rt_dynamic_t *restrict dynamic, miko_program_s *restrict program, const char *restrict name, miko_count_t argc, const miko_program_argv_t *restrict argv);
typedef miko_rt_s* (*miko_program_interrupt_f)(miko_rt_s *restrict runtime, miko_interrupt_t interrupt);

struct miko_program_s {
	miko_wink_gomi_s *gomi;
	const miko_env_runtime_s *runtime;
	miko_program_symbol_f symbol;
	miko_program_interrupt_f interrupt;
};

#endif
