#ifndef _miko_type_program_argv_h_
#define _miko_type_program_argv_h_

#include "miko.type.base.h"
#include "miko.type.var.h"

typedef struct miko_program_argv_t miko_program_argv_t;

struct miko_program_argv_t {
	miko_index_t action;
	miko_index_t major;
	miko_index_t minor;
	miko_var_t var;
};

#endif
