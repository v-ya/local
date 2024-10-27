#ifndef _miko_base_miko_iset_h_
#define _miko_base_miko_iset_h_

#include "miko.h"
#include <vattr.h>

struct miko_iset_s {
	refer_string_t name;
	vattr_s *depend;       // miko.iset.name        => (refer_string_t)     miko.iset.name
	vattr_s *type;         // miko.type.name        => (refer_string_t)     miko.type.name
	vattr_s *instruction;  // miko.instruction.name => (miko_instruction_s) miko.instruction
};

#endif
