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
	if ((r = (elfin_inst_s *) refer_alloz(sizeof(elfin_inst_s))))
	{
		refer_set_free(r, (refer_free_f) elfin_inst_free_func);
		#define d_allocer(_id)  if (!(r->allocer[elfin_item_id__##_id] = elfin_inner_alloc_item_inst__##_id())) goto label_fail
		d_allocer(elfin);
		d_allocer(section_null);
		d_allocer(section_nobits);
		d_allocer(section_program);
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

const char* elfin_item_name(const elfin_item_s *restrict item)
{
	return item->name;
}

const char* elfin_item_type(const elfin_item_s *restrict item)
{
	return item->inst->type;
}
