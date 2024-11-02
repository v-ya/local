#ifndef _miko_type_env_runtime_h_
#define _miko_type_env_runtime_h_

#include "miko.type.table.h"

typedef struct miko_env_runtime_s miko_env_runtime_s;

struct miko_env_runtime_s {
	const miko_table_s *segment;      // (refer_string_t)
	const miko_table_s *action;       // (refer_string_t)
	const miko_table_s *interrupt;    // (refer_string_t)
	const miko_table_s *major;        // (miko_major_s *)
	const miko_table_s *process;      // (miko_process_s *)
	const miko_table_s *instruction;  // (miko_instruction_s *)
};

#endif
