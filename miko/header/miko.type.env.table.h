#ifndef _miko_type_env_table_h_
#define _miko_type_env_table_h_

#include "miko.type.table.h"

typedef struct miko_env_table_s miko_env_table_s;

struct miko_env_table_s {
	const miko_table_s *segment;
	const miko_table_s *action;
	const miko_table_s *major;
	const miko_table_s *instruction;
};

#endif
