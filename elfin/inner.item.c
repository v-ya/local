#include "inner.item.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst(void)
{
	return (struct elfin_item_inst_s *) refer_alloz(sizeof(struct elfin_item_inst_s));
}

struct elfin_item_s* elfin_inner_link_item(struct elfin_item_s *restrict item, const struct elfin_item_inst_s *restrict inst)
{
	item->name = inst->name;
	item->inst = (const struct elfin_item_inst_s *) refer_save(inst);
	return item;
}

void elfin_inner_clear_item(struct elfin_item_s *restrict item)
{
	if (item->inst)
		refer_free(item->inst);
}

