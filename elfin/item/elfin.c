#include "item.h"
#include <vattr.h>
#include <exbuffer.h>

struct elfin_item__elfin_s {
	struct elfin_item_s item;
	struct elfin_ident_t ident;
	vattr_s *section;
	uintptr_t section_count;
	exbuffer_t section_array;
	refer_nstring_t string_section_name;
	uint32_t type;
	uint32_t machine;
	uint32_t version;
	uint32_t flags;
	uint64_t entry_address;
};

#define r  ((struct elfin_item__elfin_s *) item)

static struct elfin_item_s* get_ident(struct elfin_item_s *restrict item, struct elfin_ident_t *restrict ident)
{
	*ident = r->ident;
	return item;
}
static struct elfin_item_s* set_ident(struct elfin_item_s *restrict item, const struct elfin_ident_t *restrict ident)
{
	r->ident = *ident;
	return item;
}

static struct elfin_item_s* get_ident_class(struct elfin_item_s *restrict item, uint32_t *restrict class)
{
	*class = (uint32_t) r->ident.class;
	return item;
}
static struct elfin_item_s* set_ident_class(struct elfin_item_s *restrict item, uint32_t class)
{
	r->ident.class = (uint8_t) class;
	return item;
}

static struct elfin_item_s* get_ident_data(struct elfin_item_s *restrict item, uint32_t *restrict data)
{
	*data = (uint32_t) r->ident.data;
	return item;
}
static struct elfin_item_s* set_ident_data(struct elfin_item_s *restrict item, uint32_t data)
{
	r->ident.data = (uint8_t) data;
	return item;
}

static struct elfin_item_s* get_ident_version(struct elfin_item_s *restrict item, uint32_t *restrict version)
{
	*version = (uint32_t) r->ident.version;
	return item;
}
static struct elfin_item_s* set_ident_version(struct elfin_item_s *restrict item, uint32_t version)
{
	r->ident.version = (uint8_t) version;
	return item;
}

static struct elfin_item_s* get_ident_osabi(struct elfin_item_s *restrict item, uint32_t *restrict osabi)
{
	*osabi = (uint32_t) r->ident.osabi;
	return item;
}
static struct elfin_item_s* set_ident_osabi(struct elfin_item_s *restrict item, uint32_t osabi)
{
	r->ident.osabi = (uint8_t) osabi;
	return item;
}

static struct elfin_item_s* get_ident_abiversion(struct elfin_item_s *restrict item, uint32_t *restrict abiversion)
{
	*abiversion = (uint32_t) r->ident.abiversion;
	return item;
}
static struct elfin_item_s* set_ident_abiversion(struct elfin_item_s *restrict item, uint32_t abiversion)
{
	r->ident.abiversion = (uint8_t) abiversion;
	return item;
}

static struct elfin_item_s* get_header_type(struct elfin_item_s *restrict item, uint32_t *restrict type)
{
	*type = r->type;
	return item;
}
static struct elfin_item_s* set_header_type(struct elfin_item_s *restrict item, uint32_t type)
{
	r->type = type;
	return item;
}

static struct elfin_item_s* get_header_machine(struct elfin_item_s *restrict item, uint32_t *restrict machine)
{
	*machine = r->machine;
	return item;
}
static struct elfin_item_s* set_header_machine(struct elfin_item_s *restrict item, uint32_t machine)
{
	r->machine = machine;
	return item;
}

static struct elfin_item_s* get_header_version(struct elfin_item_s *restrict item, uint32_t *restrict version)
{
	*version = r->version;
	return item;
}
static struct elfin_item_s* set_header_version(struct elfin_item_s *restrict item, uint32_t version)
{
	r->version = version;
	return item;
}

static struct elfin_item_s* get_header_entry_address(struct elfin_item_s *restrict item, uint64_t *restrict entry_address)
{
	*entry_address = r->entry_address;
	return item;
}
static struct elfin_item_s* set_header_entry_address(struct elfin_item_s *restrict item, uint64_t entry_address)
{
	r->entry_address = entry_address;
	return item;
}

static struct elfin_item_s* get_header_flags(struct elfin_item_s *restrict item, uint32_t *restrict flags)
{
	*flags = r->flags;
	return item;
}
static struct elfin_item_s* set_header_flags(struct elfin_item_s *restrict item, uint32_t flags)
{
	r->flags = flags;
	return item;
}

static struct elfin_item_s* append_child_section(struct elfin_item_s *restrict item, struct elfin_item_s *section)
{
	elfin_item_func__get_section_name_f get_name;
	refer_nstring_t name;
	const char *restrict key;
	if ((get_name = section->inst->func.get_section_name) && get_name(section, &name))
	{
		if (name) key = name->string;
		else key = "";
		if (vattr_insert_tail(r->section, key, section))
		{
			r->section_count += 1;
			return item;
		}
	}
	return NULL;
}

static struct elfin_item_s* find_child_section(struct elfin_item_s *restrict item, const char *restrict section_name, struct elfin_item_s **restrict p_section)
{
	if (!section_name)
		section_name = "";
	if ((*p_section = (struct elfin_item_s *) vattr_get_first(r->section, section_name)))
		return item;
	return NULL;
}

static struct elfin_item_s* get_child_section_array(struct elfin_item_s *restrict item, struct elfin_item_s ***restrict p_section_array, uintptr_t *restrict section_count)
{
	struct elfin_item_s **p;
	vattr_vlist_t *restrict vl;
	*section_count = r->section_count;
	if ((*p_section_array = p = (struct elfin_item_s **) exbuffer_need(&r->section_array, r->section_count * sizeof(struct elfin_item_s *))))
	{
		for (vl = r->section->vattr; vl; vl = vl->vattr_next)
			*p++ = (struct elfin_item_s *) vl->value;
		return item;
	}
	return NULL;
}

static struct elfin_item_s* get_child_section_header_string_table_name(struct elfin_item_s *restrict item, refer_nstring_t *restrict section_name)
{
	*section_name = r->string_section_name;
	return item;
}

static struct elfin_item_s* set_child_section_header_string_table_name(struct elfin_item_s *restrict item, refer_nstring_t section_name)
{
	if (r->string_section_name)
		refer_free(r->string_section_name);
	r->string_section_name = (refer_nstring_t) refer_save(section_name);
	return item;
}

#undef r

static void elfin_item__elfin_free_func(struct elfin_item__elfin_s *restrict r)
{
	exbuffer_uini(&r->section_array);
	if (r->section)
		refer_free(r->section);
	if (r->string_section_name)
		refer_free(r->string_section_name);
	elfin_inner_clear_item(&r->item);
}

static struct elfin_item_s* elfin_item__elfin_alloc(const struct elfin_item_inst_s *restrict inst)
{
	struct elfin_item__elfin_s *restrict r;
	if ((r = (struct elfin_item__elfin_s *) refer_alloz(sizeof(struct elfin_item__elfin_s))))
	{
		refer_set_free(r, (refer_free_f) elfin_item__elfin_free_func);
		if (exbuffer_init(&r->section_array, 0) && (r->section = vattr_alloc()))
			return elfin_inner_link_item(&r->item, inst);
		refer_free(r);
	}
	return NULL;
}

#include "../elfin.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst__elfin(void)
{
	struct elfin_item_inst_s *restrict r;
	if ((r = (struct elfin_item_inst_s *) refer_alloz(sizeof(struct elfin_item_inst_s))))
	{
		r->name = elfin_name_elfin;
		r->type = elfin_type_elfin;
		r->alloc = elfin_item__elfin_alloc;
		#define d_func(_id) r->func._id = _id
		d_func(get_ident);
		d_func(set_ident);
		d_func(get_ident_class);
		d_func(set_ident_class);
		d_func(get_ident_data);
		d_func(set_ident_data);
		d_func(get_ident_version);
		d_func(set_ident_version);
		d_func(get_ident_osabi);
		d_func(set_ident_osabi);
		d_func(get_ident_abiversion);
		d_func(set_ident_abiversion);
		d_func(get_header_type);
		d_func(set_header_type);
		d_func(get_header_machine);
		d_func(set_header_machine);
		d_func(get_header_version);
		d_func(set_header_version);
		d_func(get_header_entry_address);
		d_func(set_header_entry_address);
		d_func(get_header_flags);
		d_func(set_header_flags);
		d_func(append_child_section);
		d_func(find_child_section);
		d_func(get_child_section_array);
		d_func(get_child_section_header_string_table_name);
		d_func(set_child_section_header_string_table_name);
		#undef d_func
	}
	return r;
}
