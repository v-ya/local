#include "section.h"

#define r  ((struct elfin_item_section_s *) item)

static struct elfin_item_s* get_section_name(struct elfin_item_s *restrict item, refer_nstring_t *restrict name)
{
	*name = r->name;
	return item;
}
static struct elfin_item_s* set_section_name(struct elfin_item_s *restrict item, refer_nstring_t name)
{
	if (r->name)
		refer_free(r->name);
	r->name = (refer_nstring_t) refer_save(name);
	return item;
}

static struct elfin_item_s* get_section_type(struct elfin_item_s *restrict item, uint32_t *restrict type)
{
	*type = r->type;
	return item;
}
static struct elfin_item_s* set_section_type(struct elfin_item_s *restrict item, uint32_t type)
{
	r->type = type;
	return item;
}

static struct elfin_item_s* get_section_flags(struct elfin_item_s *restrict item, uint64_t *restrict flags)
{
	*flags = r->flags;
	return item;
}
static struct elfin_item_s* set_section_flags(struct elfin_item_s *restrict item, uint64_t flags)
{
	r->flags = flags;
	return item;
}

static struct elfin_item_s* get_section_exec_addr(struct elfin_item_s *restrict item, uint64_t *restrict exec_addr)
{
	*exec_addr = r->exec_addr;
	return item;
}
static struct elfin_item_s* set_section_exec_addr(struct elfin_item_s *restrict item, uint64_t exec_addr)
{
	r->exec_addr = exec_addr;
	return item;
}

static struct elfin_item_s* get_section_link(struct elfin_item_s *restrict item, refer_nstring_t *restrict section_name)
{
	*section_name = r->link;
	return item;
}
static struct elfin_item_s* set_section_link(struct elfin_item_s *restrict item, refer_nstring_t section_name)
{
	if (r->link)
		refer_free(r->link);
	r->link = (refer_nstring_t) refer_save(section_name);
	return item;
}

static struct elfin_item_s* get_section_info(struct elfin_item_s *restrict item, refer_nstring_t *restrict section_name)
{
	*section_name = r->info;
	return item;
}
static struct elfin_item_s* set_section_info(struct elfin_item_s *restrict item, refer_nstring_t section_name)
{
	if (r->info)
		refer_free(r->info);
	r->info = (refer_nstring_t) refer_save(section_name);
	return item;
}

static struct elfin_item_s* get_section_alignment(struct elfin_item_s *restrict item, uint64_t *restrict alignment)
{
	*alignment = r->alignment;
	return item;
}
static struct elfin_item_s* set_section_alignment(struct elfin_item_s *restrict item, uint64_t alignment)
{
	r->alignment = alignment;
	return item;
}

static struct elfin_item_s* get_section_entry_size(struct elfin_item_s *restrict item, uint64_t *restrict entry_size)
{
	*entry_size = r->entry_size;
	return item;
}
static struct elfin_item_s* set_section_entry_size(struct elfin_item_s *restrict item, uint64_t entry_size)
{
	r->entry_size = entry_size;
	return item;
}

#undef r

void elfin_inner_clear_item_section(struct elfin_item_section_s *restrict section)
{
	if (section->name)
		refer_free(section->name);
	if (section->link)
		refer_free(section->link);
	if (section->info)
		refer_free(section->info);
	elfin_inner_clear_item(&section->item);
}

struct elfin_item_s* elfin_inner_link_item_section(struct elfin_item_section_s *restrict section, const struct elfin_item_inst_s *restrict inst)
{
	section->name = NULL;
	section->link = NULL;
	section->info = NULL;
	return elfin_inner_link_item(&section->item, inst);
}

#include "../elfin.h"

void elfin_inner_init_item_section_inst(struct elfin_item_inst_s *restrict inst)
{
	inst->type = elfin_type_section;
	#define d_func(_id) inst->func._id = _id
	d_func(get_section_name);
	d_func(set_section_name);
	d_func(get_section_type);
	d_func(set_section_type);
	d_func(get_section_flags);
	d_func(set_section_flags);
	d_func(get_section_exec_addr);
	d_func(set_section_exec_addr);
	d_func(get_section_link);
	d_func(set_section_link);
	d_func(get_section_info);
	d_func(set_section_info);
	d_func(get_section_alignment);
	d_func(set_section_alignment);
	d_func(get_section_entry_size);
	d_func(set_section_entry_size);
	#undef d_func
}
