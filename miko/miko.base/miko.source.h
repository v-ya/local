#ifndef _miko_base_miko_source_h_
#define _miko_base_miko_source_h_

#include "miko.h"
#include <rbtree.h>

typedef struct miko_source_impl_s miko_source_impl_s;

struct miko_source_impl_s {
	miko_source_s source;   // miko.source
	refer_nstring_t lines;  // miko.source.replace("\r\n" => 0, "\t\v" => ' ')
	rbtree_t *pos;          // pos_line_start => row_of_source
};

#endif
