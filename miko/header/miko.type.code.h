#ifndef _miko_type_code_h_
#define _miko_type_code_h_

#include <refer.h>
#include "miko.type.base.h"
#include "miko.type.instr.h"

typedef struct miko_code_builder_s miko_code_builder_s;
typedef struct miko_code_s miko_code_s;

struct miko_code_s {
	const miko_instr_t *instr_array;
	miko_count_t instr_count;
};

#endif
