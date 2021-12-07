#ifndef _elfin_inner_inst_h_
#define _elfin_inner_inst_h_

#include "inner.item.h"

enum elfin_item_id_t {
	elfin_item_id__elfin,
	elfin_item_id__section_null,
	elfin_item_id__section_nobits,
	elfin_item_id__section_program,
	elfin_item_id__section_symbol,
	elfin_item_id__section_string,
	elfin_item_id__section_rela,
	elfin_item_id__section_rel,
	elfin_item_id__entry_symbol,
	elfin_item_id__entry_rela,
	elfin_item_id__entry_rel,
	elfin_item_id_number
};

struct elfin_inst_s {
	const struct elfin_item_inst_s *allocer[elfin_item_id_number];
};

static inline struct elfin_item_s* elfin_inner_alloc_item(const struct elfin_inst_s *restrict inst, enum elfin_item_id_t id)
{
	if ((uint32_t) id < elfin_item_id_number)
		return inst->allocer[id]->alloc(inst->allocer[id]);
	return NULL;
}

#endif
