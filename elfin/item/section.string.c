#include "section.h"

struct elfin_item__section_string_s {
	struct elfin_item_section_s section;
};

static void elfin_item__section_string_free_func(struct elfin_item__section_string_s *restrict r)
{
	elfin_inner_clear_item_section(&r->section);
}

static struct elfin_item_s* elfin_item__section_string_alloc(const struct elfin_item_inst_s *restrict inst)
{
	struct elfin_item__section_string_s *restrict r;
	if ((r = (struct elfin_item__section_string_s *) refer_alloz(sizeof(struct elfin_item__section_string_s))))
	{
		refer_set_free(r, (refer_free_f) elfin_item__section_string_free_func);
		r->section.type = elfin_section_type__string_table;
		r->section.flags = 0;
		r->section.alignment = 1;
		r->section.entry_size = 0;
		return elfin_inner_link_item_section(&r->section, inst);
	}
	return NULL;
}

#include "../elfin.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_string(void)
{
	struct elfin_item_inst_s *restrict r;
	if ((r = (struct elfin_item_inst_s *) refer_alloz(sizeof(struct elfin_item_inst_s))))
	{
		r->name = elfin_name_section_string;
		r->alloc = elfin_item__section_string_alloc;
		elfin_inner_init_item_section_inst(r);
	}
	return r;
}
