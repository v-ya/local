#ifndef _elfin_inner_inst_h_
#define _elfin_inner_inst_h_

#include "elfin.h"

struct elfin_item_inst_s;

struct elfin_inst_s {
	const struct elfin_item_inst_s *allocer[elfin_item_id_number];
};

#endif
