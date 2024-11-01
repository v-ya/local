#ifndef _miko_type_table_h_
#define _miko_type_table_h_

#include <refer.h>
#include "miko.type.base.h"

typedef struct miko_table_s miko_table_s;

struct miko_table_s {
	refer_t *table;
	miko_count_t count;
};

#endif
