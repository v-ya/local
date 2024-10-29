#ifndef _miko_base_miko_iset_h_
#define _miko_base_miko_iset_h_

#include "miko.h"
#include <vattr.h>

struct miko_iset_s {
	refer_string_t name;   // miko.iset.name
	vattr_s *depend;       // miko.iset.name        => (refer_string_t)       miko.iset.name
	vattr_s *score;        // miko.score.name       => (refer_string_t)       miko.score.name
	vattr_s *style;        // miko.style.name       => (refer_string_t)       miko.style.name
	vattr_s *major;        // miko.major.name       => (miko_major_s *)       miko.major
	vattr_s *instruction;  // miko.instruction.name => (miko_instruction_s *) miko.instruction
};

#endif
