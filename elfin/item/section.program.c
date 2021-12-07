#include "section.h"
#include <exbuffer.h>
#include <memory.h>

struct elfin_item__section_program_s {
	struct elfin_item_section_s section;
	exbuffer_t data;
};

#define r  ((struct elfin_item__section_program_s *) item)

static struct elfin_item_s* get_section_size(struct elfin_item_s *restrict item, uint64_t *restrict size)
{
	*size = r->section.size;
	return item;
}

static struct elfin_item_s* clear_section_data(struct elfin_item_s *restrict item)
{
	r->section.size = 0;
	r->data.used = 0;
	return item;
}

static struct elfin_item_s* append_section_data(struct elfin_item_s *restrict item, const void *data, uintptr_t size, uintptr_t align, uint64_t *restrict offset)
{
	uintptr_t need_size;
	if (!align) align = 1;
	need_size = r->data.used + align - 1;
	need_size -= need_size % align;
	if (exbuffer_need(&r->data, need_size))
	{
		if (need_size > r->data.used)
			memset(r->data.data + r->data.used, 0, need_size - r->data.used);
		if (offset) *offset = need_size;
		if (size)
		{
			if (!exbuffer_need(&r->data, need_size + size))
				goto label_fail;
			memcpy(r->data.data + need_size, data, size);
			need_size += size;
		}
		r->section.size = r->data.used = need_size;
		return item;
	}
	label_fail:
	return NULL;
}

static struct elfin_item_s* get_section_data(struct elfin_item_s *restrict item, void **restrict p_data, uintptr_t *restrict size)
{
	*p_data = (void *) r->data.data;
	*size = r->data.used;
	return item;
}

#undef r

static void elfin_item__section_program_free_func(struct elfin_item__section_program_s *restrict r)
{
	exbuffer_uini(&r->data);
	elfin_inner_clear_item_section(&r->section);
}

static struct elfin_item_s* elfin_item__section_program_alloc(const struct elfin_item_inst_s *restrict inst)
{
	struct elfin_item__section_program_s *restrict r;
	if ((r = (struct elfin_item__section_program_s *) refer_alloz(sizeof(struct elfin_item__section_program_s))))
	{
		refer_set_free(r, (refer_free_f) elfin_item__section_program_free_func);
		if (exbuffer_init(&r->data, 0))
		{
			r->section.type = elfin_section_type__program_bits;
			r->section.flags = 0;
			r->section.alignment = 16;
			r->section.entry_size = 0;
			return elfin_inner_link_item_section(&r->section, inst);
		}
		refer_free(r);
	}
	return NULL;
}

#include "../elfin.h"

const struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_program(void)
{
	struct elfin_item_inst_s *restrict r;
	if ((r = (struct elfin_item_inst_s *) refer_alloz(sizeof(struct elfin_item_inst_s))))
	{
		r->name = elfin_name_section_program;
		r->alloc = elfin_item__section_program_alloc;
		elfin_inner_init_item_section_inst(r);
		r->func.get_section_size = get_section_size;
		r->func.clear_section_data = clear_section_data;
		r->func.append_section_data = append_section_data;
		r->func.get_section_data = get_section_data;
	}
	return r;
}
