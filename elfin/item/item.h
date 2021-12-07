#ifndef _elfin_inner_item_item_h_
#define _elfin_inner_item_item_h_

#include "../inner.item.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst__elfin(void);
struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_null(void);
struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_nobits(void);
struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_program(void);
struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_symbol(void);
struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_string(void);
struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_rela(void);
struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_rel(void);
struct elfin_item_inst_s* elfin_inner_alloc_item_inst__entry_symbol(void);
struct elfin_item_inst_s* elfin_inner_alloc_item_inst__entry_rela(void);
struct elfin_item_inst_s* elfin_inner_alloc_item_inst__entry_rel(void);

#endif
