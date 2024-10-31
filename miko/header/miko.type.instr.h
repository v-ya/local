#ifndef _miko_type_instr_h_
#define _miko_type_instr_h_

#include "miko.type.rt.h"
#include "miko.type.form.h"
#include "miko.type.access.h"

typedef struct miko_instr_t miko_instr_t;

typedef miko_rt_s* (*miko_instr_f)(const miko_instr_t *restrict instr, miko_form_table_t segment, miko_rt_s *restrict runtime);

struct miko_instr_t {
	miko_index_t instruction;
	miko_instr_f instr_func;
	refer_t instr_data;
	uintptr_t count;
	miko_access_t argv[];
};

#endif
