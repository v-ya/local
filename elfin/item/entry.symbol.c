#include "item.h"
#include "../inner.inst.h"
#include <vattr.h>
#include <exbuffer.h>

struct elfin_item__entry_symbol_s {
	struct elfin_item_s item;
	refer_nstring_t symbol_name;
	uint32_t symbol_type;
	uint32_t symbol_bind;
	uint32_t symbol_visibility;
	uint32_t section_res_index;
	struct elfin_item_s *section_program;
	uint64_t addr;
	uint64_t size;
};

#define r  ((struct elfin_item__entry_symbol_s *) item)

static struct elfin_item_s* get_entry_symbol_name(struct elfin_item_s *restrict item, refer_nstring_t *restrict name)
{
	*name = r->symbol_name;
	return item;
}
static struct elfin_item_s* set_entry_symbol_name(struct elfin_item_s *restrict item, refer_nstring_t name)
{
	if (r->symbol_name)
		refer_free(r->symbol_name);
	r->symbol_name = (refer_nstring_t) refer_save(name);
	return item;
}

static struct elfin_item_s* get_entry_symbol_type(struct elfin_item_s *restrict item, uint32_t *restrict type)
{
	*type = r->symbol_type;
	return item;
}
static struct elfin_item_s* set_entry_symbol_type(struct elfin_item_s *restrict item, uint32_t type)
{
	r->symbol_type = type;
	return item;
}

static struct elfin_item_s* get_entry_symbol_bind(struct elfin_item_s *restrict item, uint32_t *restrict bind)
{
	*bind = r->symbol_bind;
	return item;
}
static struct elfin_item_s* set_entry_symbol_bind(struct elfin_item_s *restrict item, uint32_t bind)
{
	r->symbol_bind = bind;
	return item;
}

static struct elfin_item_s* get_entry_symbol_visibility(struct elfin_item_s *restrict item, uint32_t *restrict visibility)
{
	*visibility = r->symbol_visibility;
	return item;
}
static struct elfin_item_s* set_entry_symbol_visibility(struct elfin_item_s *restrict item, uint32_t visibility)
{
	r->symbol_visibility = visibility;
	return item;
}

static struct elfin_item_s* get_entry_symbol_section_res_index(struct elfin_item_s *restrict item, uint32_t *restrict section_res_index)
{
	*section_res_index = r->section_res_index;
	return item;
}
static struct elfin_item_s* set_entry_symbol_section_res_index(struct elfin_item_s *restrict item, uint32_t section_res_index)
{
	r->section_res_index = section_res_index;
	return item;
}

static struct elfin_item_s* get_entry_symbol_program(struct elfin_item_s *restrict item, struct elfin_item_s **restrict p_section_program)
{
	*p_section_program = r->section_program;
	return item;
}
static struct elfin_item_s* set_entry_symbol_program(struct elfin_item_s *restrict item, struct elfin_item_s *section_program)
{
	if (!section_program || section_program->inst->item_id == elfin_item_id__section_program)
	{
		if (r->section_program)
			refer_free(r->section_program);
		r->section_program = (struct elfin_item_s *) refer_save(section_program);
		return item;
	}
	return NULL;
}

static struct elfin_item_s* get_entry_symbol_range(struct elfin_item_s *restrict item, uint64_t *restrict addr, uint64_t *restrict size)
{
	if (addr) *addr = r->addr;
	if (size) *size = r->size;
	return item;
}
static struct elfin_item_s* set_entry_symbol_range(struct elfin_item_s *restrict item, uint64_t addr, uint64_t size)
{
	r->addr = addr;
	r->size = size;
	return item;
}

static struct elfin_item_s* get_entry_symbol(struct elfin_item_s *restrict item, refer_nstring_t *restrict name, uint32_t *restrict type, uint32_t *restrict bind, uint32_t *restrict visibility, uint32_t *restrict section_res_index, struct elfin_item_s **restrict p_section_program, uint64_t *restrict addr, uint64_t *restrict size)
{
	if (name) *name = r->symbol_name;
	if (type) *type = r->symbol_type;
	if (bind) *bind = r->symbol_bind;
	if (visibility) *visibility = r->symbol_visibility;
	if (section_res_index) *section_res_index = r->section_res_index;
	if (p_section_program) *p_section_program = r->section_program;
	if (addr) *addr = r->addr;
	if (size) *size = r->size;
	return item;
}
static struct elfin_item_s* set_entry_symbol(struct elfin_item_s *restrict item, refer_nstring_t name, uint32_t type, uint32_t bind, uint32_t visibility, uint32_t section_res_index, struct elfin_item_s *section_program, uint64_t addr, uint64_t size)
{
	if (!section_program || section_program->inst->item_id == elfin_item_id__section_program)
	{
		if (r->symbol_name)
			refer_free(r->symbol_name);
		r->symbol_name = (refer_nstring_t) refer_save(name);
		r->symbol_type = type;
		r->symbol_bind = bind;
		r->symbol_visibility = visibility;
		r->section_res_index = section_res_index;
		if (r->section_program)
			refer_free(r->section_program);
		r->section_program = (struct elfin_item_s *) refer_save(section_program);
		r->addr = addr;
		r->size = size;
		return item;
	}
	return NULL;
}

#undef r

static void elfin_item__entry_symbol_free_func(struct elfin_item__entry_symbol_s *restrict r)
{
	if (r->symbol_name)
		refer_free(r->symbol_name);
	if (r->section_program)
		refer_free(r->section_program);
	elfin_inner_clear_item(&r->item);
}

static struct elfin_item_s* elfin_item__entry_symbol_alloc(const struct elfin_item_inst_s *restrict inst)
{
	struct elfin_item__entry_symbol_s *restrict r;
	if ((r = (struct elfin_item__entry_symbol_s *) refer_alloz(sizeof(struct elfin_item__entry_symbol_s))))
	{
		refer_set_free(r, (refer_free_f) elfin_item__entry_symbol_free_func);
		return elfin_inner_link_item(&r->item, inst);
	}
	return NULL;
}

#include "../elfin.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst__entry_symbol(void)
{
	struct elfin_item_inst_s *restrict r;
	if ((r = (struct elfin_item_inst_s *) refer_alloz(sizeof(struct elfin_item_inst_s))))
	{
		r->name = elfin_name_entry_symbol;
		r->type = elfin_type_entry;
		r->alloc = elfin_item__entry_symbol_alloc;
		#define d_func(_id) r->func._id = _id
		d_func(get_entry_symbol_name);
		d_func(set_entry_symbol_name);
		d_func(get_entry_symbol_type);
		d_func(set_entry_symbol_type);
		d_func(get_entry_symbol_bind);
		d_func(set_entry_symbol_bind);
		d_func(get_entry_symbol_visibility);
		d_func(set_entry_symbol_visibility);
		d_func(get_entry_symbol_section_res_index);
		d_func(set_entry_symbol_section_res_index);
		d_func(get_entry_symbol_program);
		d_func(set_entry_symbol_program);
		d_func(get_entry_symbol_range);
		d_func(set_entry_symbol_range);
		d_func(get_entry_symbol);
		d_func(set_entry_symbol);
		#undef d_func
	}
	return r;
}
