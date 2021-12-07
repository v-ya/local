#ifndef _elfin_inner_item_item_h_
#define _elfin_inner_item_item_h_

#include "../inner.item.h"

const struct elfin_item_inst_s* elfin_inner_alloc_item_inst__elfin(void);
const struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_null(void);
const struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_nobits(void);
const struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_program(void);

#endif
