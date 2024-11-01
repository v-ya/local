#ifndef _miko_base_miko_table_h_
#define _miko_base_miko_table_h_

#include "miko.h"
#include <vattr.h>

typedef struct miko_table_impl_s miko_table_impl_s;
typedef struct miko_table_range_s miko_table_range_s;

struct miko_table_impl_s {
	miko_table_s table;
	miko_vector_s_t vector;
	vattr_s *range;
};

struct miko_table_range_s {
	miko_index_t index;
	miko_count_t count;
};

#endif
