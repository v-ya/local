#ifndef _miko_base_miko_iset_h_
#define _miko_base_miko_iset_h_

#include "miko.h"
#include <vattr.h>

struct miko_iset_s {
	refer_string_t name;   // miko.iset.name
	vattr_s *depend;       // miko.iset.name        => (refer_string_t)       miko.iset.name
	vattr_s *segment;      // miko.segment.name     => (refer_string_t)       miko.segment.name
	vattr_s *action;       // miko.action.name      => (refer_string_t)       miko.action.name
	vattr_s *interrupt;    // miko.interrupt.name   => (refer_string_t)       miko.interrupt.name
	vattr_s *major;        // miko.major.name       => (miko_major_s *)       miko.major
	vattr_s *process;      // miko.process.name     => (miko_process_s *)     miko.process
	vattr_s *instruction;  // miko.instruction.name => (miko_instruction_s *) miko.instruction[]
};

#endif
