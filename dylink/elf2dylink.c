#include "elf2dylink.h"
#include <elfin/elfin.h>
#include <elfin/elfin.item.h>
#include <hashmap.h>
#include <vattr.h>
#include <exbuffer.h>
#include <string.h>
#include "dylink.h"

struct elf2dylink_s {
	elfin_inst_s *inst;
	elfin_item_s *elf;
	elfin_item_s *dylink_string;
	elfin_item_s *dylink_image;
	elfin_item_s *dylink_export;
	vattr_s *dylink_import;
	hashmap_t used_section;      // section_index => ...
	hashmap_t used_import;       // symbol_id     => (elfin_item_s *entry_symbol)
	hashmap_t used_export;       // symbol_id     => (elfin_item_s *entry_symbol)
	exbuffer_t data;
	elfin_item_s **section_array;
	uintptr_t section_count;
	uintptr_t import_number;
};

static void elf2dylink_free_func(elf2dylink_s *restrict r)
{
	hashmap_uini(&r->used_section);
	hashmap_uini(&r->used_import);
	hashmap_uini(&r->used_export);
	exbuffer_uini(&r->data);
	if (r->elf) refer_free(r->elf);
	if (r->dylink_string) refer_free(r->dylink_string);
	if (r->dylink_image) refer_free(r->dylink_image);
	if (r->dylink_import) refer_free(r->dylink_import);
	if (r->dylink_export) refer_free(r->dylink_export);
	if (r->inst) refer_free(r->inst);
}

static elf2dylink_s* elf2dylink_alloc(void)
{
	elf2dylink_s *restrict r;
	if ((r = (elf2dylink_s *) refer_alloz(sizeof(elf2dylink_s))))
	{
		refer_set_free(r, (refer_free_f) elf2dylink_free_func);
		if (hashmap_init(&r->used_section) &&
			hashmap_init(&r->used_import) &&
			hashmap_init(&r->used_export) &&
			exbuffer_init(&r->data, 0) &&
			(r->inst = elfin_inst_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

static inline uint64_t elf2dylink_symbol_id(uintptr_t section_index, uint32_t symbol_index)
{
	return ((uint64_t) section_index << 32) | (uint64_t) symbol_index;
}

static elf2dylink_s* elf2dylink_need_section(elf2dylink_s *restrict r, uintptr_t index)
{
	refer_nstring_t name;
	if (elfin_item__get_section_name(r->section_array[index], &name) &&
		hashmap_put_head(&r->used_section, (uint64_t) index, NULL, NULL) &&
		hashmap_put_name(&r->used_section, name?name->string:"", (const void *) index, NULL))
		return r;
	return NULL;
}

static elf2dylink_s* elf2dylink_need_section_by_name(elf2dylink_s *restrict r, refer_nstring_t name, uintptr_t *restrict index)
{
	hashmap_vlist_t *restrict vl;
	const char *restrict key;
	elfin_item_s *s;
	uintptr_t i;
	key = name?name->string:"";
	if ((vl = hashmap_find_name(&r->used_section, key)))
	{
		i = (uintptr_t) vl->value;
		label_okay:
		if (index) *index = i;
		return r;
	}
	else if ((elfin_item__find_child_section(r->elf, key, &s)))
	{
		for (i = 0; i < r->section_count; ++i)
		{
			if (r->section_array[i] == s)
			{
				if (elf2dylink_need_section(r, i))
					goto label_okay;
				break;
			}
		}
	}
	return NULL;
}

static inline elf2dylink_s* elf2dylink_need_import(elf2dylink_s *restrict r, uintptr_t section_index, uint32_t symbol_index, elfin_item_s *entry_symbol)
{
	if (hashmap_put_head(&r->used_import, elf2dylink_symbol_id(section_index, symbol_index), entry_symbol, NULL))
		return r;
	return NULL;
}

static inline elf2dylink_s* elf2dylink_need_export(elf2dylink_s *restrict r, uintptr_t section_index, uint32_t symbol_index, elfin_item_s *entry_symbol)
{
	if (hashmap_put_head(&r->used_export, elf2dylink_symbol_id(section_index, symbol_index), entry_symbol, NULL))
		return r;
	return NULL;
}

static elf2dylink_s* elf2dylink_select_elf_need_relo(elf2dylink_s *restrict r, elfin_item_s *relo)
{
	refer_nstring_t name;
	uintptr_t symbol_table_index;
	elfin_item_s *symbol_table;
	elfin_item_s *program;
	elfin_item_s **symbol_array;
	elfin_item_s **relo_array;
	uintptr_t symbol_count;
	uintptr_t relo_count;
	uintptr_t i;
	uint32_t symbol_index;
	uint32_t section_res_index;
	if (!(elfin_item__get_section_link(relo, &name)) || !name)
		goto label_fail;
	if (!elf2dylink_need_section_by_name(r, name, &symbol_table_index))
		goto label_fail;
	symbol_table = r->section_array[symbol_table_index];
	if (!(elfin_item__get_child_entry_array(symbol_table, &symbol_array, &symbol_count)))
		goto label_fail;
	if (!(elfin_item__get_child_entry_array(relo, &relo_array, &relo_count)))
		goto label_fail;
	for (i = 0; i < relo_count; ++i)
	{
		if (!elfin_item__get_entry_relo_symbol_index(relo_array[i], &symbol_index))
			goto label_fail;
		if (symbol_index < symbol_count)
		{
			if (!elfin_item__get_entry_symbol_program(symbol_array[symbol_index], &program))
				goto label_fail;
			if (program)
			{
				if (!(elfin_item__get_section_name(program, &name)))
					goto label_fail;
				if (!elf2dylink_need_section_by_name(r, name, NULL))
					goto label_fail;
			}
			else
			{
				if (!(elfin_item__get_entry_symbol_section_res_index(symbol_array[symbol_index], &section_res_index)))
					goto label_fail;
				if (section_res_index == elfin_section_res_index__undef)
				{
					if (!elf2dylink_need_import(r, symbol_table_index, symbol_index, symbol_array[symbol_index]))
						goto label_fail;
				}
			}
		}
	}
	return r;
	label_fail:
	return NULL;
}

static elf2dylink_s* elf2dylink_select_elf_need_symbol(elf2dylink_s *restrict r, uintptr_t symbol_table_index)
{
	refer_nstring_t name;
	elfin_item_s *symbol_table;
	elfin_item_s *program;
	elfin_item_s **symbol_array;
	uintptr_t symbol_count;
	uintptr_t symbol_index;
	uint32_t symbol_bind;
	symbol_table = r->section_array[symbol_table_index];
	if (!(elfin_item__get_child_entry_array(symbol_table, &symbol_array, &symbol_count)))
		goto label_fail;
	for (symbol_index = 0; symbol_index < symbol_count; ++symbol_index)
	{
		if (!elfin_item__get_entry_symbol_program(symbol_array[symbol_index], &program))
			goto label_fail;
		if (program)
		{
			if (!(elfin_item__get_section_name(program, &name)))
				goto label_fail;
			if (!(elfin_item__get_entry_symbol_bind(symbol_array[symbol_index], &symbol_bind)))
				goto label_fail;
			if (symbol_bind == elfin_symbol_bind__global ||
				symbol_bind == elfin_symbol_bind__weak)
			{
				if (!elf2dylink_need_section_by_name(r, name, NULL))
					goto label_fail;
				if (!elf2dylink_need_export(r, symbol_table_index, (uint32_t) symbol_index, symbol_array[symbol_index]))
					goto label_fail;
			}
		}
	}
	return r;
	label_fail:
	return NULL;
}

static elf2dylink_s* elf2dylink_select_elf_need(elf2dylink_s *restrict r)
{
	refer_nstring_t name;
	uintptr_t i;
	uintptr_t new_need_relo;
	uint64_t flags;
	uint32_t type;
	hashmap_clear(&r->used_section);
	hashmap_clear(&r->used_import);
	hashmap_clear(&r->used_export);
	if (!(elfin_item__get_child_section_array(r->elf, &r->section_array, &r->section_count)))
		goto label_fail;
	// select execinstr and symbol
	for (i = 0; i < r->section_count; ++i)
	{
		if (!elfin_item__get_section_flags(r->section_array[i], &flags))
			goto label_fail;
		if (flags & elfin_section_flag__execinstr)
		{
			if (!elf2dylink_need_section(r, i))
				goto label_fail;
		}
		if (!elfin_item__get_section_type(r->section_array[i], &type))
			goto label_fail;
		if (type == elfin_section_type__symbol_table)
		{
			if (!elf2dylink_need_section(r, i))
				goto label_fail;
			if (!elf2dylink_select_elf_need_symbol(r, i))
				goto label_fail;
		}
	}
	// loop select relo
	do {
		new_need_relo = 0;
		for (i = 0; i < r->section_count; ++i)
		{
			if (!hashmap_find_head(&r->used_section, (uint64_t) i))
			{
				if (!elfin_item__get_section_type(r->section_array[i], &type))
					goto label_fail;
				if (type == elfin_section_type__rela || type == elfin_section_type__rel)
				{
					if (!elfin_item__get_section_info(r->section_array[i], &name))
						goto label_fail;
					if (hashmap_find_name(&r->used_section, name?name->string:""))
					{
						// new need relo
						if (!elf2dylink_need_section(r, i))
							goto label_fail;
						new_need_relo += 1;
						if (!elf2dylink_select_elf_need_relo(r, r->section_array[i]))
							goto label_fail;
					}
				}
			}
		}
	} while (new_need_relo);
	return r;
	label_fail:
	return NULL;
}

elf2dylink_s* elf2dylink_load_by_memory(const void *restrict data, uintptr_t size)
{
	elf2dylink_s *restrict r;
	if ((r = elf2dylink_alloc()))
	{
		if ((r->elf = elfin_load_elfin_by_memory(r->inst, data, size)) &&
			elf2dylink_select_elf_need(r))
			return r;
		refer_free(r);
	}
	return NULL;
}

elf2dylink_s* elf2dylink_load_by_path(const char *restrict path)
{
	elf2dylink_s *restrict r;
	if ((r = elf2dylink_alloc()))
	{
		if ((r->elf = elfin_load_elfin_by_path(r->inst, path)) &&
			elf2dylink_select_elf_need(r))
			return r;
		refer_free(r);
	}
	return NULL;
}

static elf2dylink_s* elf2dylink_build_dylink_merge_program(elf2dylink_s *restrict r, hashmap_t *restrict program_offset, uint64_t *restrict takeup_size)
{
	uintptr_t i;
	void *data;
	uintptr_t size;
	uint64_t offset, bss_size;
	uint32_t type;
	for (i = 0; i < r->section_count; ++i)
	{
		if (hashmap_find_head(&r->used_section, (uint64_t) i) &&
			elfin_item__get_section_data(r->section_array[i], &data, &size) && size)
		{
			if (!elfin_item__append_section_data(r->dylink_image, data, size, 16, &offset))
				goto label_fail;
			if (!hashmap_set_head(program_offset,
				(uint64_t) (uintptr_t) r->section_array[i],
				(const void *) (uintptr_t) offset, NULL))
				goto label_fail;
		}
	}
	if (!elfin_item__append_section_data(r->dylink_image, NULL, 0, 16, &offset))
		goto label_fail;
	for (i = 0; i < r->section_count; ++i)
	{
		if (hashmap_find_head(&r->used_section, (uint64_t) i) &&
			elfin_item__get_section_type(r->section_array[i], &type) &&
			type == elfin_section_type__no_bits)
		{
			if (!elfin_item__get_section_size(r->section_array[i], &bss_size))
				goto label_fail;
			if (!hashmap_set_head(program_offset,
				(uint64_t) (uintptr_t) r->section_array[i],
				(const void *) (uintptr_t) offset, NULL))
				goto label_fail;
			// 16 bytes align
			offset = (offset + bss_size + 0xf) & ~(uint64_t) 0xf;
		}
	}
	*takeup_size = offset;
	return r;
	label_fail:
	return NULL;
}

static elf2dylink_s* elf2dylink_build_dylink_do_relo_append(elf2dylink_s *restrict r, refer_nstring_t symbol, uint32_t type, uint64_t offset, int64_t addend)
{
	elf2dylink_s *rr;
	elfin_item_s *e;
	rr = NULL;
	if (symbol && (e = elfin_create_entry_rela(r->inst)))
	{
		if (elfin_item__set_entry_relo(e, offset, addend, type, 0) &&
			elfin_item__put_string_with_length(r->dylink_string, symbol->string, symbol->length) &&
			vattr_insert_tail(r->dylink_import, symbol->string, e))
		{
			r->import_number += 1;
			rr = r;
		}
		refer_free(e);
	}
	return rr;
}

static elf2dylink_s* elf2dylink_build_dylink_do_export_append(elf2dylink_s *restrict r, refer_nstring_t symbol, uint64_t offset)
{
	elf2dylink_s *rr;
	elfin_item_s *e;
	rr = NULL;
	if (symbol && (e = elfin_create_entry_symbol(r->inst)))
	{
		if (elfin_item__set_entry_symbol_name(e, symbol) &&
			elfin_item__set_entry_symbol_range(e, offset, 0) &&
			elfin_item__put_string_with_length(r->dylink_string, symbol->string, symbol->length) &&
			elfin_item__append_child_entry(r->dylink_export, e))
			rr = r;
		refer_free(e);
	}
	return rr;
}

static elf2dylink_s* elf2dylink_build_dylink_do_relo(elf2dylink_s *restrict r, hashmap_t *restrict program_offset, dylink_set_f set_func)
{
	elfin_item_s *relo, *program, *symbol;
	uintptr_t i, j, section_index;
	refer_nstring_t name;
	elfin_item_s **symbol_array;
	elfin_item_s **relo_array;
	uintptr_t symbol_count;
	uintptr_t relo_count;
	uintptr_t offset, size;
	void *ptr;
	uint8_t *restrict image_data;
	uint64_t r_offset, symbol_addr;
	int64_t r_addend;
	uint32_t type, symbol_index;
	if (!elfin_item__get_section_data(r->dylink_image, &ptr, &size))
		goto label_fail;
	image_data = (uint8_t *) ptr;
	for (i = 0; i < r->section_count; ++i)
	{
		if (hashmap_find_head(&r->used_section, (uint64_t) i) &&
			elfin_item__get_section_type(relo = r->section_array[i], &type) &&
			(type == elfin_section_type__rela || type == elfin_section_type__rel))
		{
			// info => data
			if (!elfin_item__get_section_info(relo, &name))
				goto label_fail;
			section_index = (uintptr_t) hashmap_get_name(&r->used_section, name?name->string:"");
			program = r->section_array[section_index];
			if (!elfin_item__get_section_size(program, &size))
				goto label_fail;
			offset = (uintptr_t) hashmap_get_head(program_offset, (uint64_t) (uintptr_t) program);
			// link => symbol
			if (!elfin_item__get_section_link(relo, &name))
				goto label_fail;
			section_index = (uintptr_t) hashmap_get_name(&r->used_section, name?name->string:"");
			symbol = r->section_array[section_index];
			if (!elfin_item__get_child_entry_array(symbol, &symbol_array, &symbol_count))
				goto label_fail;
			// relo
			if (!elfin_item__get_child_entry_array(relo, &relo_array, &relo_count))
				goto label_fail;
			for (j = 0; j < relo_count; ++j)
			{
				if (!elfin_item__get_entry_relo(relo_array[j], &r_offset, &r_addend, &type, &symbol_index))
					goto label_fail;
				if (symbol_index >= symbol_count)
					goto label_fail;
				if (!elfin_item__get_entry_symbol_program(symbol_array[symbol_index], &program))
					goto label_fail;
				if (r_offset >= size)
					goto label_fail;
				if (program)
				{
					// inner link
					if (!elfin_item__get_entry_symbol_range(symbol_array[symbol_index], &symbol_addr, NULL))
						goto label_fail;
					symbol_addr += (uintptr_t) hashmap_get_head(program_offset, (uint64_t) (uintptr_t) program);
					if (set_func(type, image_data + offset + r_offset, r_addend, image_data + symbol_addr, NULL))
					{
						if (!hashmap_find_head(&r->used_export, elf2dylink_symbol_id(section_index, symbol_index)))
							goto label_fail;
						// maybe less plt
						goto label_wait_import;
					}
				}
				else
				{
					// maybe need extern symbol
					label_wait_import:
					if (!elfin_item__get_entry_symbol_name(symbol_array[symbol_index], &name))
						goto label_fail;
					if (!elf2dylink_build_dylink_do_relo_append(r, name, type, offset + r_offset, r_addend))
						goto label_fail;
				}
			}
		}
	}
	return r;
	label_fail:
	return NULL;
}

static elf2dylink_s* elf2dylink_build_dylink_do_export(elf2dylink_s *restrict r, hashmap_t *restrict program_offset)
{
	elfin_item_s *symbol, *program;
	uintptr_t i;
	refer_nstring_t name;
	elfin_item_s **symbol_array;
	uintptr_t symbol_count;
	uint64_t symbol_addr;
	uint32_t type, symbol_index;
	for (i = 0; i < r->section_count; ++i)
	{
		if (hashmap_find_head(&r->used_section, (uint64_t) i) &&
			elfin_item__get_section_type(symbol = r->section_array[i], &type) &&
			type == elfin_section_type__symbol_table)
		{
			if (!elfin_item__get_child_entry_array(symbol, &symbol_array, &symbol_count))
				goto label_fail;
			for (symbol_index = 0; symbol_index < symbol_count; ++symbol_index)
			{
				if (!hashmap_find_head(&r->used_export, elf2dylink_symbol_id(i, symbol_index)))
					continue;
				if (!elfin_item__get_entry_symbol_program(symbol_array[symbol_index], &program))
					goto label_fail;
				if (!program)
					goto label_fail;
				if (!elfin_item__get_entry_symbol_name(symbol_array[symbol_index], &name))
					goto label_fail;
				if (!elfin_item__get_entry_symbol_range(symbol_array[symbol_index], &symbol_addr, NULL))
					goto label_fail;
				symbol_addr += (uintptr_t) hashmap_get_head(program_offset, (uint64_t) (uintptr_t) program);
				if (!elf2dylink_build_dylink_do_export_append(r, name, symbol_addr))
					goto label_fail;
			}
		}
	}
	return r;
	label_fail:
	return NULL;
}

static elf2dylink_s* elf2dylink_build_dylink_build(elf2dylink_s *restrict r, uint64_t takeup_size, const char *restrict magic)
{
	dylink_header_t header;
	void *d_string, *d_image;
	uint8_t *restrict data;
	dylink_isym_t *restrict pi;
	dylink_esym_t *restrict pe;
	vattr_vslot_t *restrict vslot;
	vattr_vlist_t *restrict vlist;
	elfin_item_s **d_export;
	refer_nstring_t name;
	uint64_t offset;
	uintptr_t n_string, n_import, n_export, n_image, n_size, i, si;
	r->data.used = 0;
	if (!elfin_item__get_section_data(r->dylink_string, &d_string, &n_string))
		goto label_fail;
	n_import = r->import_number;
	if (!elfin_item__get_child_entry_array(r->dylink_export, &d_export, &n_export))
		goto label_fail;
	if (!elfin_item__get_section_data(r->dylink_image, &d_image, &n_image))
		goto label_fail;
	// header
	strcpy(memset(header.machine, 0, sizeof(header.machine)), magic);
	header.version = 1;
	header.header_size = sizeof(header);
	header.img_takeup = takeup_size;
	header.img_offset = 0;
	header.img_size = (uint32_t) n_image;
	header.strpool_offset = 0;
	header.strpool_size = (uint32_t) n_string;
	header.isym_offset = 0;
	header.isym_number = (uint32_t) n_import;
	header.esym_offset = 0;
	header.esym_number = (uint32_t) n_export;
	n_size = sizeof(header);
	header.strpool_offset = (uint32_t) n_size;
	n_size = (n_size + n_string + 0xf) & ~(uint64_t) 0xf;
	header.isym_offset = (uint32_t) n_size;
	n_size = (n_size + n_import * sizeof(dylink_isym_t) + 0xf) & ~(uint64_t) 0xf;
	header.esym_offset = (uint32_t) n_size;
	n_size = (n_size + n_export * sizeof(dylink_esym_t) + 0xf) & ~(uint64_t) 0xf;
	header.img_offset = (uint32_t) n_size;
	n_size = (n_size + n_image + 0xf) & ~(uint64_t) 0xf;
	// alloc
	if (!(data = (uint8_t *) exbuffer_need(&r->data, n_size)))
		goto label_fail;
	memset(data, 0, n_size);
	// set header
	memcpy(data, &header, sizeof(header));
	// set string
	memcpy(data + header.strpool_offset, d_string, n_string);
	// set import
	pi = (dylink_isym_t *) (data + header.isym_offset);
	i = 0;
	while ((vlist = vattr_first(r->dylink_import)))
	{
		vslot = vlist->vslot;
		if (!elfin_item__get_string_index(r->dylink_string, vslot->key, &si))
			goto label_fail;
		si += header.strpool_offset;
		do {
			pi[i].name_offset = (uint32_t) si;
			if (!elfin_item__get_entry_relo((elfin_item_s *) vlist->value, &pi[i].offset, &pi[i].addend, &pi[i].type, NULL))
				goto label_fail;
			++i;
		} while ((vlist = vlist->vslot_next));
		vattr_delete_vslot(vslot);
	}
	// set export
	pe = (dylink_esym_t *) (data + header.esym_offset);
	for (i = 0; i < n_export; ++i)
	{
		if (!elfin_item__get_entry_symbol_name(d_export[i], &name))
			goto label_fail;
		if (!name)
			goto label_fail;
		if (!elfin_item__get_string_index(r->dylink_string, name->string, &si))
			goto label_fail;
		if (!elfin_item__get_entry_symbol_range(d_export[i], &offset, NULL))
			goto label_fail;
		pe[i].name_offset = (uint32_t) si + header.strpool_offset;
		pe[i].offset = (uint32_t) offset;
	}
	// set image
	memcpy(data + header.img_offset, d_image, n_image);
	r->data.used = n_size;
	return r;
	label_fail:
	return NULL;
}

static const char* elf2dylink_build_dylink_get_magic_and_set_func(elf2dylink_s *restrict r, dylink_set_f *restrict set_func)
{
	uint32_t bits, mechine;
	if (elfin_item__get_ident_class(r->elf, &bits) &&
		elfin_item__get_header_machine(r->elf, &mechine))
	{
		if (bits == elfin_ident_class__64)
		{
			switch (mechine)
			{
				case elfin_machine__x86_64:
					*set_func = m_x86_64_dylink_set;
					return dylink_mechine_x86_64;
				default:
					break;
			}
		}
	}
	*set_func = NULL;
	return NULL;
}

void* elf2dylink_build_dylink(elf2dylink_s *restrict r, uintptr_t *size)
{
	if (!r->dylink_string) r->dylink_string = elfin_create_section_string(r->inst);
	else elfin_item__clear_section_data(r->dylink_string);
	if (!r->dylink_image) r->dylink_image = elfin_create_section_program(r->inst);
	else elfin_item__clear_section_data(r->dylink_image);
	if (!r->dylink_export) r->dylink_export = elfin_create_section_symbol(r->inst);
	else elfin_item__clear_section_data(r->dylink_export);
	if (!r->dylink_import) r->dylink_import = vattr_alloc();
	else vattr_clear(r->dylink_import);
	if (r->dylink_string && r->dylink_image && r->dylink_image && r->dylink_export)
	{
		hashmap_t h1;
		hashmap_t *program_offset;
		void *rdata;
		dylink_set_f set_func;
		const char *restrict magic;
		uint64_t takeup_size;
		rdata = NULL;
		program_offset = hashmap_init(&h1);
		if (program_offset)
		{
			if ((magic = elf2dylink_build_dylink_get_magic_and_set_func(r, &set_func)) &&
				elf2dylink_build_dylink_merge_program(r, program_offset, &takeup_size) &&
				elf2dylink_build_dylink_do_relo(r, program_offset, set_func) &&
				elf2dylink_build_dylink_do_export(r, program_offset) &&
				elfin_item__build_string_map(r->dylink_string) &&
				elf2dylink_build_dylink_build(r, takeup_size, magic))
			{
				*size = r->data.used;
				rdata = (void *) r->data.data;
			}
		}
		if (program_offset) hashmap_uini(program_offset);
		return rdata;
	}
	return NULL;
}

#include <stdio.h>

static inline const char* elf2dylink_dump_elf_string_nstring(refer_nstring_t ns)
{
	return ns?ns->string:"";
}

static const char* elf2dylink_dump_elf_string_section_type(uint32_t type)
{
	static const char *const a[elfin_section_type_number] = {
		[elfin_section_type__null]               = "null",
		[elfin_section_type__program_bits]       = "program",
		[elfin_section_type__symbol_table]       = "symbol",
		[elfin_section_type__string_table]       = "string",
		[elfin_section_type__rela]               = "rela",
		[elfin_section_type__hash]               = "hash",
		[elfin_section_type__dynamic]            = "dynamic",
		[elfin_section_type__note]               = "note",
		[elfin_section_type__no_bits]            = "nobits",
		[elfin_section_type__rel]                = "rel",
		[elfin_section_type__shlib]              = "shlib",
		[elfin_section_type__dynamic_symbol]     = "dynamic_symbol",
		[elfin_section_type__init_array]         = "init_array",
		[elfin_section_type__fini_array]         = "fini_array",
		[elfin_section_type__preinit_array]      = "preinit_array",
		[elfin_section_type__group]              = "group",
		[elfin_section_type__symbol_table_shndx] = "symbol_table_shndx",
	};
	if (type < elfin_section_type_number)
		return a[type];
	return "?";
}

static const char* elf2dylink_dump_elf_string_symbol_bind(uint32_t v)
{
	static const char *const a[elfin_symbol_bind_number] = {
		[elfin_symbol_bind__local]  = "local",
		[elfin_symbol_bind__global] = "global",
		[elfin_symbol_bind__weak]   = "weak",
	};
	if (v < elfin_symbol_bind_number)
		return a[v];
	return "";
}

static const char* elf2dylink_dump_elf_string_symbol_type(uint32_t v)
{
	static const char *const a[elfin_symbol_type_number] = {
		[elfin_symbol_type__no_type] = "notype",
		[elfin_symbol_type__object]  = "object",
		[elfin_symbol_type__func]    = "func",
		[elfin_symbol_type__section] = "section",
		[elfin_symbol_type__file]    = "file",
		[elfin_symbol_type__common]  = "common",
		[elfin_symbol_type__tls]     = "tls",
	};
	if (v < elfin_symbol_type_number)
		return a[v];
	return "";
}

static const char* elf2dylink_dump_elf_string_symbol_visibility(uint32_t v)
{
	static const char *const a[elfin_symbol_visibility_number] = {
		[elfin_symbol_visibility__default]   = "default",
		[elfin_symbol_visibility__internal]  = "internal",
		[elfin_symbol_visibility__hidden]    = "hidden",
		[elfin_symbol_visibility__protected] = "protected",
	};
	if (v < elfin_symbol_visibility_number)
		return a[v];
	return "";
}

static void elf2dylink_dump_elf_section(elf2dylink_s *restrict r, uintptr_t section_index)
{
	elfin_item_s *s;
	refer_nstring_t name, link, info;
	uint32_t type;
	s = r->section_array[section_index];
	if (elfin_item__get_section_type(s, &type) &&
		elfin_item__get_section_name(s, &name) &&
		elfin_item__get_section_link(s, &link) &&
		elfin_item__get_section_info(s, &info))
	{
		if (link || info)
			printf("session[%2lu] %18s %c(%24s) link: %s, info: %s\n",
				section_index, elf2dylink_dump_elf_string_section_type(type),
				hashmap_find_head(&r->used_section, (uint64_t) section_index)?'*':' ',
				elf2dylink_dump_elf_string_nstring(name),
				elf2dylink_dump_elf_string_nstring(link),
				elf2dylink_dump_elf_string_nstring(info));
		else printf("session[%2lu] %18s %c(%24s)\n",
				section_index, elf2dylink_dump_elf_string_section_type(type),
				hashmap_find_head(&r->used_section, (uint64_t) section_index)?'*':' ',
				elf2dylink_dump_elf_string_nstring(name));
	}
}

static void elf2dylink_dump_elf_symbol(elf2dylink_s *restrict r, uintptr_t section_index)
{
	elfin_item_s *s;
	elfin_item_s **symbol_array;
	uintptr_t symbol_count;
	refer_nstring_t name;
	uintptr_t i;
	uint64_t addr, size;
	uint32_t type, bind, vis, is_import, is_export;
	s = r->section_array[section_index];
	if (hashmap_find_head(&r->used_section, (uint64_t) section_index) &&
		elfin_item__get_section_type(s, &type) && type == elfin_section_type__symbol_table)
	{
		if (elfin_item__get_section_name(s, &name))
			printf("[%lu](%s)\n", section_index, elf2dylink_dump_elf_string_nstring(name));
		if ((elfin_item__get_child_entry_array(s, &symbol_array, &symbol_count)))
		{
			for (i = 0; i < symbol_count; ++i)
			{
				is_import = !!hashmap_find_head(&r->used_import, elf2dylink_symbol_id(section_index, i));
				is_export = !!hashmap_find_head(&r->used_export, elf2dylink_symbol_id(section_index, i));
				if ((is_import || is_export) && elfin_item__get_entry_symbol(symbol_array[i],
					&name, &type, &bind, &vis, NULL, NULL, &addr, &size))
				{
					printf("symbol[%3lu] @%8lu+%-8lu %9s %6s %7s %c%s\n",
						i, addr, size,
						elf2dylink_dump_elf_string_symbol_visibility(vis),
						elf2dylink_dump_elf_string_symbol_bind(bind),
						elf2dylink_dump_elf_string_symbol_type(type),
						" -+*"[(is_export << 1) | is_import],
						elf2dylink_dump_elf_string_nstring(name));
				}
			}
		}
	}
}

void elf2dylink_dump_elf(elf2dylink_s *restrict r)
{
	uintptr_t i;
	for (i = 0; i < r->section_count; ++i)
		elf2dylink_dump_elf_section(r, i);
	for (i = 0; i < r->section_count; ++i)
		elf2dylink_dump_elf_symbol(r, i);
}

void dylink_dump(const uint8_t *restrict r, uintptr_t size)
{
	const dylink_header_t *restrict h;
	const dylink_isym_t *restrict ih;
	const dylink_esym_t *restrict eh;
	uint32_t i, n;
	if (size >= sizeof(dylink_header_t))
	{
		h = (dylink_header_t *) r;
		printf(
			"         machine: %s\n"
			"         version: %u\n"
			"     header_size: %u\n"
			"      img_takeup: %lu\n"
			"      img_offset: %u\n"
			"        img_size: %u\n"
			"  strpool_offset: %u\n"
			"    strpool_size: %u\n"
			"     isym_offset: %u\n"
			"     isym_number: %u\n"
			"     esym_offset: %u\n"
			"     esym_number: %u\n",
			h->machine,
			h->version,
			h->header_size,
			h->img_takeup,
			h->img_offset,
			h->img_size,
			h->strpool_offset,
			h->strpool_size,
			h->isym_offset,
			h->isym_number,
			h->esym_offset,
			h->esym_number
		);
		if (h->img_offset + h->img_size > size) printf("waring: h->img_offset + h->img_size > size\n");
		if (h->strpool_offset + h->strpool_size > size) printf("waring: h->strpool_offset + h->strpool_size > size\n");
		if (h->isym_offset + h->isym_number * sizeof(dylink_isym_t) > size) printf("waring: h->isym_offset + h->isym_number * sizeof(dylink_isym_t) > size\n");
		else
		{
			ih = (dylink_isym_t *) (r + h->isym_offset);
			n = h->isym_number;
			printf("isym[%u]\n", n);
			for (i = 0; i < n; ++i)
			{
				printf(
					"\t[%u]: type: %2u, offset: %8ld, addend: %6ld, (%4u): %s\n",
					i,
					ih->type,
					ih->offset,
					ih->addend,
					ih->name_offset,
					(ih->name_offset >= h->strpool_offset && ih->name_offset < h->strpool_offset + h->strpool_size)?
						(char *) (r + ih->name_offset):
						"???"
				);
				++ih;
			}
		}
		if (h->esym_offset + h->esym_number * sizeof(dylink_esym_t) > size) printf("waring: h->esym_offset + h->esym_number * sizeof(dylink_esym_t) > size\n");
		else
		{
			eh = (dylink_esym_t *) (r + h->esym_offset);
			n = h->esym_number;
			printf("esym[%u]\n", n);
			for (i = 0; i < n; ++i)
			{
				printf(
					"\t[%u]: offset: %8d, (%4u): %s\n",
					i,
					eh->offset,
					eh->name_offset,
					(eh->name_offset >= h->strpool_offset && eh->name_offset < h->strpool_offset + h->strpool_size)?
						(char *) (r + eh->name_offset):
						"???"
				);
				++eh;
			}
		}
	}
}
