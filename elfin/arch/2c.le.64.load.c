#include "arch.h"
#include "../elfin.64.h"
#include <memory.h>

static struct elfin_item_s* load_elfin(struct elfin_item_s *restrict r, const struct elfin_64_header_t *restrict header)
{
	const struct elfin_item_inst_s *restrict p;
	p = r->inst;
	if (!p->func.set_ident(r, &header->ident)) goto label_fail;
	if (!p->func.set_header_type(r, (uint32_t) header->type)) goto label_fail;
	if (!p->func.set_header_machine(r, (uint32_t) header->machine)) goto label_fail;
	if (!p->func.set_header_version(r, header->version)) goto label_fail;
	if (!p->func.set_header_entry_address(r, header->addr_entry)) goto label_fail;
	if (!p->func.set_header_flags(r, header->flags)) goto label_fail;
	return r;
	label_fail:
	return NULL;
}

static const struct elfin_64_section_header_t* load_section_check(const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict count, uintptr_t *restrict string_index)
{
	uint64_t offset;
	uintptr_t entry_size, i, n, si;
	const struct elfin_64_section_header_t *restrict sh;
	offset = ((const struct elfin_64_header_t *) data)->offset_section_header;
	entry_size = (uintptr_t) ((const struct elfin_64_header_t *) data)->section_header_entry_size;
	n = (uintptr_t) ((const struct elfin_64_header_t *) data)->section_header_entry_count;
	si = (uintptr_t) ((const struct elfin_64_header_t *) data)->section_header_string_index;
	if (entry_size != sizeof(struct elfin_64_section_header_t)) goto label_fail;
	if (offset > size) goto label_fail;
	if (entry_size * n > size - offset) goto label_fail;
	if (si > n) goto label_fail;
	sh = (const struct elfin_64_section_header_t *) (data + offset);
	for (i = 0; i < n; ++i)
	{
		if (sh[i].offset > size) goto label_fail;
		if (sh[i].size > size - sh[i].offset) goto label_fail;
	}
	*count = n;
	*string_index = si;
	return sh;
	label_fail:
	return NULL;
}

static const char* load_section_name(const struct elfin_64_section_header_t *restrict ss, const uint8_t *restrict data, uint32_t name, uintptr_t *restrict length)
{
	const char *restrict s, *restrict e;
	if (ss->type == elfin_section_type__string_table && (uint64_t) name < ss->size)
	{
		s = (const char *) (data + ss->offset + name);
		if ((e = memchr(s, 0, ss->size - name)))
		{
			if (length) *length = (uintptr_t) e - (uintptr_t) s;
			return s;
		}
	}
	return NULL;
}

static refer_nstring_t load_section_name_dump_nstring(const struct elfin_64_section_header_t *restrict ss, const uint8_t *restrict data, uint32_t name)
{
	const char *restrict s;
	uintptr_t length;
	if ((s = load_section_name(ss, data, name, &length)))
		return refer_dump_nstring_with_length(s, length);
	return NULL;
}

static struct elfin_item_s* load_section_empty(struct elfin_item_s *restrict r, const struct elfin_inst_s *restrict inst, const uint8_t *restrict data, const struct elfin_64_section_header_t *restrict sh, uintptr_t count, uintptr_t string_index)
{
	const struct elfin_item_inst_s *restrict p, *restrict q;
	struct elfin_item_s *restrict s;
	refer_nstring_t name;
	uintptr_t i;
	enum elfin_item_id_t id;
	p = r->inst;
	s = NULL;
	name = NULL;
	for (i = 0; i < count; ++i)
	{
		switch (sh[i].type)
		{
			case elfin_section_type__null:
				id = elfin_item_id__section_null;
				break;
			case elfin_section_type__program_bits:
			case elfin_section_type__note:
			case elfin_section_type__dynamic:
			case elfin_section_type__init_array:
			case elfin_section_type__fini_array:
				id = elfin_item_id__section_program;
				break;
			case elfin_section_type__symbol_table:
				id = elfin_item_id__section_symbol;
				break;
			case elfin_section_type__string_table:
				id = elfin_item_id__section_string;
				break;
			case elfin_section_type__rela:
				id = elfin_item_id__section_rela;
				break;
			case elfin_section_type__no_bits:
				id = elfin_item_id__section_nobits;
				break;
			case elfin_section_type__rel:
				id = elfin_item_id__section_rel;
				break;
			default: id = elfin_item_id_number;
		}
		if (id < elfin_item_id_number)
		{
			if (!(s = elfin_inner_alloc_item(inst, id)))
				goto label_fail;
			q = s->inst;
			if (!(name = load_section_name_dump_nstring(sh + string_index, data, sh[i].name)))
				goto label_fail;
			if (!q->func.set_section_name(s, name)) goto label_fail;
			if (!q->func.set_section_type(s, sh[i].type)) goto label_fail;
			if (!q->func.set_section_flags(s, sh[i].flags)) goto label_fail;
			if (!q->func.set_section_exec_addr(s, sh[i].addr)) goto label_fail;
			if (!q->func.set_section_alignment(s, sh[i].addr_align)) goto label_fail;
			if (!q->func.set_section_entry_size(s, sh[i].entry_size)) goto label_fail;
			if (!p->func.append_child_section(r, s)) goto label_fail;
			refer_free(s);
			refer_free(name);
			s = NULL;
			name = NULL;
		}
	}
	return r;
	label_fail:
	if (s) refer_free(s);
	if (name) refer_free(name);
	return NULL;
}

static struct elfin_item_s* load_section_link_and_info(struct elfin_item_s *restrict r, const uint8_t *restrict data, const struct elfin_64_section_header_t *restrict sh, uintptr_t count, uintptr_t string_index)
{
	const struct elfin_item_inst_s *restrict p, *restrict q;
	struct elfin_item_s *s, *t;
	refer_nstring_t section_name;
	const char *name, *link, *info;
	uintptr_t i, n_link, n_info;
	p = r->inst;
	for (i = 0; i < count; ++i)
	{
		link = info = NULL;
		if ((n_link = (uintptr_t) sh[i].link) < count && sh[n_link].type != elfin_section_type__null)
			link = load_section_name(sh + string_index, data, sh[n_link].name, &n_link);
		if ((n_info = (uintptr_t) sh[i].info) < count && sh[n_info].type != elfin_section_type__null)
			info = load_section_name(sh + string_index, data, sh[n_info].name, &n_info);
		if (!(name = load_section_name(sh + string_index, data, sh[i].name, NULL)))
			goto label_fail;
		if (link && info && p->func.find_child_section(r, name, &s))
		{
			q = s->inst;
			if (link)
			{
				if (p->func.find_child_section(r, link, &t))
				{
					if (!t->inst->func.get_section_name(t, &section_name))
						goto label_fail;
					if (!q->func.set_section_link(s, section_name))
						goto label_fail;
				}
				else
				{
					if (!(section_name = refer_dump_nstring_with_length(link, n_link)))
						goto label_fail;
					s = q->func.set_section_link(s, section_name);
					refer_free(section_name);
					if (!s) goto label_fail;
				}
			}
			if (info)
			{
				if (p->func.find_child_section(r, info, &t))
				{
					if (!t->inst->func.get_section_name(t, &section_name))
						goto label_fail;
					if (!q->func.set_section_info(s, section_name))
						goto label_fail;
				}
				else
				{
					if (!(section_name = refer_dump_nstring_with_length(info, n_info)))
						goto label_fail;
					s = q->func.set_section_info(s, section_name);
					refer_free(section_name);
					if (!s) goto label_fail;
				}
			}
		}
	}
	return r;
	label_fail:
	return NULL;
}

static struct elfin_item_s* load_section_data_symbol(struct elfin_item_s *restrict r, struct elfin_item_s *restrict elfin, const struct elfin_inst_s *restrict inst, const uint8_t *restrict data, const struct elfin_64_section_header_t *restrict sh, uintptr_t count, uintptr_t symbol_index, uintptr_t string_index)
{
	const struct elfin_64_section_header_t *restrict symbol;
	const struct elfin_64_section_header_t *restrict string;
	const struct elfin_64_symbol_entry_t *restrict se;
	const struct elfin_item_inst_s *restrict p, *restrict q;
	const char *restrict program_name;
	struct elfin_item_s *section_program;
	struct elfin_item_s *e;
	refer_nstring_t name;
	uintptr_t n;
	uint32_t section_res_index;
	symbol = sh + symbol_index;
	string = sh + symbol->link;
	if (symbol->link < count &&
		symbol->entry_size == sizeof(struct elfin_64_symbol_entry_t) &&
		string->type == elfin_section_type__string_table)
	{
		p = r->inst;
		q = elfin->inst;
		se = (const struct elfin_64_symbol_entry_t *) (data + symbol->offset);
		n = symbol->size / sizeof(struct elfin_64_symbol_entry_t);
		e = NULL;
		name = NULL;
		while (n)
		{
			if (!(e = elfin_inner_alloc_item(inst, elfin_item_id__entry_symbol)))
				goto label_fail;
			if (!(name = load_section_name_dump_nstring(string, data, se->name)))
				goto label_fail;
			section_res_index = (uint32_t) se->section_index;
			section_program = NULL;
			if (section_res_index > elfin_section_res_index__undef &&
				section_res_index < elfin_section_res_index__reserve_low)
			{
				if (section_res_index >= count)
					goto label_fail;
				if (!(program_name = load_section_name(sh + string_index, data, sh[section_res_index].name, NULL)))
					goto label_fail;
				if (!q->func.find_child_section(elfin, program_name, &section_program))
					goto label_fail;
				section_res_index = elfin_section_res_index__undef;
			}
			if (!e->inst->func.set_entry_symbol(e, name,
				elfin_64_symbol_entry__get_type(se),
				elfin_64_symbol_entry__get_bind(se),
				elfin_64_symbol_entry__get_visibility(se),
				section_res_index, section_program, se->addr, se->size))
				goto label_fail;
			if (!p->func.append_child_entry(r, e))
				goto label_fail;
			refer_free(e);
			refer_free(name);
			e = NULL;
			name = NULL;
			++se;
			--n;
		}
		return r;
		label_fail:
		if (e) refer_free(e);
		if (name) refer_free(name);
	}
	return NULL;
}

static struct elfin_item_s* load_section_data_rela(struct elfin_item_s *restrict r, const struct elfin_inst_s *restrict inst, const uint8_t *restrict data, const struct elfin_64_section_header_t *restrict relo)
{
	const struct elfin_64_rela_entry_t *restrict re;
	const struct elfin_item_inst_s *restrict p;
	struct elfin_item_s *e;
	uintptr_t n;
	if (relo->entry_size == sizeof(struct elfin_64_rela_entry_t))
	{
		p = r->inst;
		re = (const struct elfin_64_rela_entry_t *) (data + relo->offset);
		n = relo->size / sizeof(struct elfin_64_rela_entry_t);
		e = NULL;
		while (n)
		{
			if (!(e = elfin_inner_alloc_item(inst, elfin_item_id__entry_rela)))
				goto label_fail;
			if (!e->inst->func.set_entry_relo(e, re->rel.offset, re->addend,
				elfin_64_rel_entry__get_type(&re->rel),
				elfin_64_rel_entry__get_symbol(&re->rel)))
				goto label_fail;
			if (!p->func.append_child_entry(r, e))
				goto label_fail;
			refer_free(e);
			e = NULL;
			++re;
			--n;
		}
		return r;
		label_fail:
		if (e) refer_free(e);
	}
	return NULL;
}

static struct elfin_item_s* load_section_data_rel(struct elfin_item_s *restrict r, const struct elfin_inst_s *restrict inst, const uint8_t *restrict data, const struct elfin_64_section_header_t *restrict relo)
{
	const struct elfin_64_rel_entry_t *restrict re;
	const struct elfin_item_inst_s *restrict p;
	struct elfin_item_s *e;
	uintptr_t n;
	if (relo->entry_size == sizeof(struct elfin_64_rel_entry_t))
	{
		p = r->inst;
		re = (const struct elfin_64_rel_entry_t *) (data + relo->offset);
		n = relo->size / sizeof(struct elfin_64_rel_entry_t);
		e = NULL;
		while (n)
		{
			if (!(e = elfin_inner_alloc_item(inst, elfin_item_id__entry_rel)))
				goto label_fail;
			if (!e->inst->func.set_entry_relo(e, re->offset, 0,
				elfin_64_rel_entry__get_type(re),
				elfin_64_rel_entry__get_symbol(re)))
				goto label_fail;
			if (!p->func.append_child_entry(r, e))
				goto label_fail;
			refer_free(e);
			e = NULL;
			++re;
			--n;
		}
		return r;
		label_fail:
		if (e) refer_free(e);
	}
	return NULL;
}

static struct elfin_item_s* load_section_data(struct elfin_item_s *restrict r, const struct elfin_inst_s *restrict inst, const uint8_t *restrict data, const struct elfin_64_section_header_t *restrict sh, uintptr_t count, uintptr_t string_index)
{
	const struct elfin_item_inst_s *restrict p, *restrict q;
	struct elfin_item_s *s;
	const char *restrict name;
	uintptr_t i;
	p = r->inst;
	for (i = 0; i < count; ++i)
	{
		if (!(name = load_section_name(sh + string_index, data, sh[i].name, NULL)))
			goto label_fail;
		if (p->func.find_child_section(r, name, &s))
		{
			q = s->inst;
			switch (q->item_id)
			{
				case elfin_item_id__section_nobits:
					if (sh[i].size && !q->func.set_section_size(s, sh[i].size))
						goto label_fail;
					break;
				case elfin_item_id__section_program:
				case elfin_item_id__section_string:
					if (!q->func.append_section_data(s, data + sh[i].offset, sh[i].size, 0, NULL))
						goto label_fail;
					break;
				case elfin_item_id__section_symbol:
					if (!load_section_data_symbol(s, r, inst, data, sh, count, i, string_index))
						goto label_fail;
					break;
				case elfin_item_id__section_rela:
					if (!load_section_data_rela(s, inst, data, sh + i))
						goto label_fail;
					break;
				case elfin_item_id__section_rel:
					if (!load_section_data_rel(s, inst, data, sh + i))
						goto label_fail;
					break;
				default: break;
			}
		}
	}
	return r;
	label_fail:
	return NULL;
}

static struct elfin_item_s* load_section(struct elfin_item_s *restrict r, const struct elfin_inst_s *restrict inst, const uint8_t *restrict data, uintptr_t size)
{
	const struct elfin_64_section_header_t *restrict sh;
	uintptr_t n, si;
	if (!(sh = load_section_check(data, size, &n, &si)))
		goto label_fail;
	if (!load_section_empty(r, inst, data, sh, n, si))
		goto label_fail;
	if (!load_section_link_and_info(r, data, sh, n, si))
		goto label_fail;
	if (!load_section_data(r, inst, data, sh, n, si))
		goto label_fail;
	return r;
	label_fail:
	return NULL;
}

struct elfin_item_s* elfin_inner_load_2c_le_64(const struct elfin_inst_s *restrict inst, const uint8_t *restrict data, uintptr_t size)
{
	struct elfin_item_s *restrict r;
	if (size >= sizeof(struct elfin_64_header_t) &&
		(r = elfin_inner_alloc_item(inst, elfin_item_id__elfin)))
	{
		if (
			load_elfin(r, (const struct elfin_64_header_t *) data) &&
			load_section(r, inst, data, size)
		) return r;
		refer_free(r);
	}
	return NULL;
}
