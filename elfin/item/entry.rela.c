#include "item.h"
#include "../inner.inst.h"
#include <vattr.h>
#include <exbuffer.h>

struct elfin_item__entry_rela_s {
	struct elfin_item_s item;
	uint64_t offset;
	int64_t addend;
	uint32_t type;
	uint32_t symbol_index;
};

#define r  ((struct elfin_item__entry_rela_s *) item)

static struct elfin_item_s* get_entry_relo_offset(struct elfin_item_s *restrict item, uint64_t *restrict offset)
{
	*offset = r->offset;
	return item;
}
static struct elfin_item_s* set_entry_relo_offset(struct elfin_item_s *restrict item, uint64_t offset)
{
	r->offset = offset;
	return item;
}

static struct elfin_item_s* get_entry_relo_addend(struct elfin_item_s *restrict item, int64_t *restrict addend)
{
	*addend = r->addend;
	return item;
}
static struct elfin_item_s* set_entry_relo_addend(struct elfin_item_s *restrict item, int64_t addend)
{
	r->addend = addend;
	return item;
}

static struct elfin_item_s* get_entry_relo_type(struct elfin_item_s *restrict item, uint32_t *restrict r_type)
{
	*r_type = r->type;
	return item;
}
static struct elfin_item_s* set_entry_relo_type(struct elfin_item_s *restrict item, uint32_t r_type)
{
	r->type = r_type;
	return item;
}

static struct elfin_item_s* get_entry_relo_symbol_index(struct elfin_item_s *restrict item, uint32_t *restrict symbol_index)
{
	*symbol_index = r->symbol_index;
	return item;
}
static struct elfin_item_s* set_entry_relo_symbol_index(struct elfin_item_s *restrict item, uint32_t symbol_index)
{
	r->symbol_index = symbol_index;
	return item;
}

static struct elfin_item_s* get_entry_relo(struct elfin_item_s *restrict item, uint64_t *restrict offset, int64_t *restrict addend, uint32_t *restrict r_type, uint32_t *restrict symbol_index)
{
	if (offset) *offset = r->offset;
	if (addend) *addend = r->addend;
	if (r_type) *r_type = r->type;
	if (symbol_index) *symbol_index = r->symbol_index;
	return item;
}
static struct elfin_item_s* set_entry_relo(struct elfin_item_s *restrict item, uint64_t offset, int64_t addend, uint32_t r_type, uint32_t symbol_index)
{
	r->offset = offset;
	r->addend = addend;
	r->type = r_type;
	r->symbol_index = symbol_index;
	return item;
}

#undef r

static void elfin_item__entry_rela_free_func(struct elfin_item__entry_rela_s *restrict r)
{
	elfin_inner_clear_item(&r->item);
}

static struct elfin_item_s* elfin_item__entry_rela_alloc(const struct elfin_item_inst_s *restrict inst)
{
	struct elfin_item__entry_rela_s *restrict r;
	if ((r = (struct elfin_item__entry_rela_s *) refer_alloz(sizeof(struct elfin_item__entry_rela_s))))
	{
		refer_set_free(r, (refer_free_f) elfin_item__entry_rela_free_func);
		return elfin_inner_link_item(&r->item, inst);
	}
	return NULL;
}

#include "../elfin.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst__entry_rela(void)
{
	struct elfin_item_inst_s *restrict r;
	if ((r = (struct elfin_item_inst_s *) refer_alloz(sizeof(struct elfin_item_inst_s))))
	{
		r->name = elfin_name_entry_rela;
		r->type = elfin_type_entry;
		r->alloc = elfin_item__entry_rela_alloc;
		#define d_func(_id) r->func._id = _id
		d_func(get_entry_relo_offset);
		d_func(set_entry_relo_offset);
		d_func(get_entry_relo_addend);
		d_func(set_entry_relo_addend);
		d_func(get_entry_relo_type);
		d_func(set_entry_relo_type);
		d_func(get_entry_relo_symbol_index);
		d_func(set_entry_relo_symbol_index);
		d_func(get_entry_relo);
		d_func(set_entry_relo);
		#undef d_func
	}
	return r;
}
