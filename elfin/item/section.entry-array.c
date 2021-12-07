#include "section.h"
#include <exbuffer.h>

struct elfin_item_inst_section_entry_array_s {
	struct elfin_item_inst_s inst;
	uintptr_t allow_item_id;
	uint32_t default_type;
	uint64_t default_flags;
	uint64_t default_alignment;
	uint64_t default_entry_size;
};

struct elfin_item_section_entry_array_s {
	struct elfin_item_section_s section;
	uintptr_t allow_item_id;
	uintptr_t count;
	exbuffer_t entry;
};

#define r  ((struct elfin_item_section_entry_array_s *) item)

static struct elfin_item_s* clear_section_data(struct elfin_item_s *restrict item)
{
	struct elfin_item_s **restrict p;
	uintptr_t i, n;
	p = (struct elfin_item_s **) r->entry.data;
	for (i = 0, n = r->count; i < n; ++i)
		refer_free(p[i]);
	r->entry.used = 0;
	r->count = 0;
	return item;
}

static struct elfin_item_s* append_child_entry(struct elfin_item_s *restrict item, struct elfin_item_s *entry)
{
	if (entry->inst->item_id == r->allow_item_id)
	{
		if (exbuffer_append(&r->entry, &entry, sizeof(entry)))
		{
			r->count += 1;
			refer_save(entry);
			return item;
		}
	}
	return NULL;
}

static struct elfin_item_s* get_child_entry_array(struct elfin_item_s *restrict item, struct elfin_item_s ***restrict p_entry_array, uintptr_t *restrict entry_count)
{
	*p_entry_array = (struct elfin_item_s **) r->entry.data;
	*entry_count = r->count;
	return item;
}

#undef r

static void elfin_item_section_entry_array_free_func(struct elfin_item_section_entry_array_s *restrict r)
{
	clear_section_data(&r->section.item);
	exbuffer_uini(&r->entry);
	elfin_inner_clear_item_section(&r->section);
}

static struct elfin_item_s* elfin_item_section_entry_array_alloc(const struct elfin_item_inst_section_entry_array_s *restrict inst)
{
	struct elfin_item_section_entry_array_s *restrict r;
	if ((r = (struct elfin_item_section_entry_array_s *) refer_alloz(sizeof(struct elfin_item_section_entry_array_s))))
	{
		refer_set_free(r, (refer_free_f) elfin_item_section_entry_array_free_func);
		if (exbuffer_init(&r->entry, 0))
		{
			r->section.type = inst->default_type;
			r->section.flags = inst->default_flags;
			r->section.alignment = inst->default_alignment;
			r->section.entry_size = inst->default_entry_size;
			r->allow_item_id = inst->allow_item_id;
			return elfin_inner_link_item_section(&r->section, &inst->inst);
		}
		refer_free(r);
	}
	return NULL;
}

struct elfin_item_inst_s* elfin_inner_alloc_item_inst_section_entry_array(const char *restrict name, uintptr_t allow_item_id, uint32_t default_type, uint64_t default_flags, uint64_t default_alignment, uint64_t default_entry_size)
{
	struct elfin_item_inst_section_entry_array_s *restrict r;
	if ((r = (struct elfin_item_inst_section_entry_array_s *) refer_alloz(sizeof(struct elfin_item_inst_section_entry_array_s))))
	{
		r->inst.name = name;
		r->inst.alloc = (elfin_item_alloc_f) elfin_item_section_entry_array_alloc;
		elfin_inner_init_item_section_inst(&r->inst);
		r->inst.func.clear_section_data = clear_section_data;
		r->inst.func.append_child_entry = append_child_entry;
		r->inst.func.get_child_entry_array = get_child_entry_array;
		r->allow_item_id = allow_item_id;
		r->default_type = default_type;
		r->default_flags = default_flags;
		r->default_alignment = default_alignment;
		r->default_entry_size = default_entry_size;
	}
	return &r->inst;
}
