#include "section.h"

struct elfin_item__section_nobits_s {
	struct elfin_item_section_s section;
};

#define r  ((struct elfin_item__section_nobits_s *) item)

static struct elfin_item_s* get_section_size(struct elfin_item_s *restrict item, uint64_t *restrict size)
{
	*size = r->section.size;
	return item;
}
static struct elfin_item_s* set_section_size(struct elfin_item_s *restrict item, uint64_t size)
{
	r->section.size = size;
	return item;
}

#undef r

static void elfin_item__section_nobits_free_func(struct elfin_item__section_nobits_s *restrict r)
{
	elfin_inner_clear_item_section(&r->section);
}

static struct elfin_item_s* elfin_item__section_nobits_alloc(const struct elfin_item_inst_s *restrict inst)
{
	struct elfin_item__section_nobits_s *restrict r;
	if ((r = (struct elfin_item__section_nobits_s *) refer_alloz(sizeof(struct elfin_item__section_nobits_s))))
	{
		refer_set_free(r, (refer_free_f) elfin_item__section_nobits_free_func);
		r->section.type = elfin_section_type__no_bits;
		r->section.flags = 0;
		r->section.alignment = 16;
		r->section.entry_size = 0;
		return elfin_inner_link_item_section(&r->section, inst);
	}
	return NULL;
}

#include "../elfin.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_nobits(void)
{
	struct elfin_item_inst_s *restrict r;
	if ((r = (struct elfin_item_inst_s *) refer_alloz(sizeof(struct elfin_item_inst_s))))
	{
		r->name = elfin_name_section_nobits;
		r->alloc = elfin_item__section_nobits_alloc;
		elfin_inner_init_item_section_inst(r);
		r->func.get_section_size = get_section_size;
		r->func.set_section_size = set_section_size;
	}
	return r;
}
