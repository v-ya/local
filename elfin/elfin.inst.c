#include "elfin.h"
#include "inner.inst.h"
#include "item/item.h"

static void elfin_inst_free_func(elfin_inst_s *restrict r)
{
	for (uintptr_t i = 0; i < elfin_item_id_number; ++i)
		if (r->allocer[i])
			refer_free(r->allocer[i]);
}

elfin_inst_s* elfin_inst_alloc(void)
{
	elfin_inst_s *restrict r;
	struct elfin_item_inst_s *restrict inst;
	if ((r = (elfin_inst_s *) refer_alloz(sizeof(elfin_inst_s))))
	{
		refer_set_free(r, (refer_free_f) elfin_inst_free_func);
		#define d_allocer(_id)  \
			if (!(inst = elfin_inner_alloc_item_inst__##_id())) goto label_fail;\
			inst->item_id = elfin_item_id__##_id;\
			r->allocer[elfin_item_id__##_id] = inst
		d_allocer(elfin);
		d_allocer(section_null);
		d_allocer(section_nobits);
		d_allocer(section_program);
		d_allocer(section_symbol);
		d_allocer(section_string);
		d_allocer(section_rela);
		d_allocer(section_rel);
		d_allocer(entry_symbol);
		d_allocer(entry_rela);
		d_allocer(entry_rel);
		#undef d_allocer
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

elfin_item_s* elfin_create_elfin(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__elfin);
}

elfin_item_s* elfin_create_section_null(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__section_null);
}

elfin_item_s* elfin_create_section_nobits(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__section_nobits);
}

elfin_item_s* elfin_create_section_program(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__section_program);
}

elfin_item_s* elfin_create_section_symbol(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__section_symbol);
}

elfin_item_s* elfin_create_section_string(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__section_string);
}

elfin_item_s* elfin_create_section_rela(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__section_rela);
}

elfin_item_s* elfin_create_section_rel(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__section_rel);
}

elfin_item_s* elfin_create_entry_symbol(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__entry_symbol);
}

elfin_item_s* elfin_create_entry_rela(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__entry_rela);
}

elfin_item_s* elfin_create_entry_rel(const elfin_inst_s *restrict inst)
{
	return elfin_inner_alloc_item(inst, elfin_item_id__entry_rel);
}

const char* elfin_item_name(const elfin_item_s *restrict item)
{
	return item->name;
}

const char* elfin_item_type(const elfin_item_s *restrict item)
{
	return item->inst->type;
}
