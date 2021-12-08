#include "elfin.h"
#include "inner.item.h"
#include "inner.inst.h"

#define elfin_dump_indent  4

#define elfin_println(_fmt, ...)  mlog_printf(mlog, "%*c" _fmt "\n", indent, 0, ##__VA_ARGS__)

static void elfin_inner_dump(elfin_item_s *restrict item, mlog_s *mlog, uint32_t indent);

void elfin_dump(elfin_item_s *restrict item, mlog_s *mlog)
{
	elfin_inner_dump(item, mlog, 1);
}

static const char *const s_empty = "";

static void elfin_inner_dump_entry_symbol(elfin_item_s *restrict r, mlog_s *mlog, uint32_t indent, uintptr_t index)
{
	const char* elfin_inner_get__section_res_index(uint32_t v);
	const char* elfin_inner_get__symbol_bind(uint32_t v);
	const char* elfin_inner_get__symbol_type(uint32_t v);
	const char* elfin_inner_get__symbol_visibility(uint32_t v);
	refer_nstring_t name;
	uint32_t type, bind, visibility, section_res_index;
	elfin_item_s *program;
	uint64_t addr, size;
	const char *restrict symbol_name;
	const char *restrict sp_name;
	if (r->inst->func.get_entry_symbol(r, &name, &type, &bind, &visibility, &section_res_index, &program, &addr, &size))
	{
		symbol_name = name?name->string:s_empty;
		if (program && program->inst->func.get_section_name &&
			program->inst->func.get_section_name(program, &name))
			sp_name = name?name->string:s_empty;
		else sp_name = elfin_inner_get__section_res_index(section_res_index);
		elfin_println(
			"{%s}[%zu] "
			"offset: %6lu, size: %6lu, "
			"vis: %9s, bind: %6s, type: %7s, "
			"@ %16s | %s"
			, r->name, index
			, addr, size
			, elfin_inner_get__symbol_visibility(visibility)
			, elfin_inner_get__symbol_bind(bind)
			, elfin_inner_get__symbol_type(type)
			, sp_name, symbol_name
			);
	}
}

static void elfin_inner_dump_entry_relo(elfin_item_s *restrict r, mlog_s *mlog, uint32_t indent, elfin_item_s *restrict link, uintptr_t index)
{
	const char* elfin_inner_get__section_res_index(uint32_t v);
	uint64_t offset;
	int64_t addend;
	uint32_t type, symbol_index;
	const char *restrict symbol_name;
	const char *restrict sp_name;
	if (r->inst->func.get_entry_relo(r, &offset, &addend, &type, &symbol_index))
	{
		symbol_name = NULL;
		sp_name = NULL;
		if (link && link->inst->item_id == elfin_item_id__section_symbol)
		{
			elfin_item_s *symbol;
			elfin_item_s **ea;
			uintptr_t ec;
			refer_nstring_t name;
			uint32_t section_res_index;
			if (link->inst->func.get_child_entry_array(link, &ea, &ec) &&
				symbol_index < ec)
			{
				symbol = ea[symbol_index];
				if (symbol->inst->func.get_entry_symbol(symbol, &name, NULL, NULL, NULL, &section_res_index, &symbol, NULL, NULL))
				{
					symbol_name = name?name->string:s_empty;
					if (symbol && symbol->inst->func.get_section_name &&
						symbol->inst->func.get_section_name(symbol, &name))
						sp_name = name?name->string:s_empty;
					else sp_name = elfin_inner_get__section_res_index(section_res_index);
				}
			}
		}
		if (symbol_name && sp_name)
		{
			elfin_println(
				"{%s}[%zu] "
				"offset: %6lu, addend: %6ld, type: %4u, symbol_index: %4u "
				"@ %16s | %s"
				, r->name, index
				, offset, addend, type, symbol_index
				, sp_name, symbol_name
				);
		}
		else
		{
			elfin_println(
				"{%s}[%zu] "
				"offset: %6lu, addend: %6ld, type: %4u, symbol_index: %4u"
				, r->name, index
				, offset, addend, type, symbol_index
				);
		}
	}
}

static void elfin_inner_dump_entry(elfin_item_s *restrict r, mlog_s *mlog, uint32_t indent, void (*dump_entry_func)(elfin_item_s *restrict r, mlog_s *mlog, uint32_t indent, uintptr_t index))
{
	struct elfin_item_s **ea;
	uintptr_t i, ec;
	if (r->inst->func.get_child_entry_array(r, &ea, &ec))
	{
		for (i = 0; i < ec; ++i)
			dump_entry_func(ea[i], mlog, indent, i);
	}
}

static void elfin_inner_dump_entry_with_link(elfin_item_s *restrict r, mlog_s *mlog, uint32_t indent, elfin_item_s *restrict elfin, void (*dump_entry_func)(elfin_item_s *restrict r, mlog_s *mlog, uint32_t indent, elfin_item_s *restrict link, uintptr_t index))
{
	elfin_item_s *link;
	struct elfin_item_s **ea;
	uintptr_t i, ec;
	link = NULL;
	if (elfin)
	{
		refer_nstring_t link_name;
		if (r->inst->func.get_section_link(r, &link_name) && link_name)
			elfin->inst->func.find_child_section(elfin, link_name->string, &link);
	}
	if (r->inst->func.get_child_entry_array(r, &ea, &ec))
	{
		for (i = 0; i < ec; ++i)
			dump_entry_func(ea[i], mlog, indent, link, i);
	}
}

static void elfin_inner_dump_section(elfin_item_s *restrict r, mlog_s *mlog, uint32_t indent, elfin_item_s *restrict elfin)
{
	const char* elfin_inner_get__section_type(uint32_t v);
	const char* elfin_inner_get__section_flags(uint64_t flags, char *restrict buffer);
	const struct elfin_item_inst_s *restrict p;
	char buffer[512];
	refer_nstring_t str;
	uint64_t u64;
	uint32_t u32;
	p = r->inst;
	elfin_println("{%s}", r->name);
	if (p->func.get_section_name(r, &str))
		elfin_println("name:          %s", str?str->string:s_empty);
	if (p->func.get_section_type(r, &u32))
		elfin_println("type:          %u (%s)", u32, elfin_inner_get__section_type(u32));
	if (p->func.get_section_flags(r, &u64))
		elfin_println("flags:         %08lx (%s)", u64, elfin_inner_get__section_flags(u64, buffer));
	if (p->func.get_section_exec_addr(r, &u64))
		elfin_println("entry-address: 0x%lx", u64);
	if (p->func.get_section_size && p->func.get_section_size(r, &u64))
		elfin_println("size:          %lu", u64);
	if (p->func.get_section_link(r, &str))
		elfin_println("link:          %s", str?str->string:s_empty);
	if (p->func.get_section_info(r, &str))
		elfin_println("info:          %s", str?str->string:s_empty);
	if (p->func.get_section_alignment(r, &u64))
		elfin_println("alignment:     %lu", u64);
	if (p->func.get_section_entry_size(r, &u64))
		elfin_println("entry-size:    %lu", u64);
	indent += elfin_dump_indent;
	switch (p->item_id)
	{
		case elfin_item_id__section_symbol:
			elfin_inner_dump_entry(r, mlog, indent, elfin_inner_dump_entry_symbol);
			break;
		case elfin_item_id__section_rela:
		case elfin_item_id__section_rel:
			elfin_inner_dump_entry_with_link(r, mlog, indent, elfin, elfin_inner_dump_entry_relo);
			break;
		default: break;
	}
}

static void elfin_inner_dump_elfin(elfin_item_s *restrict r, mlog_s *mlog, uint32_t indent)
{
	const char* elfin_inner_get__ident_class(uint32_t v);
	const char* elfin_inner_get__ident_data(uint32_t v);
	const char* elfin_inner_get__ident_osabi(uint32_t v);
	const char* elfin_inner_get__type(uint32_t v);
	const char* elfin_inner_get__machine(uint32_t v);
	const struct elfin_item_inst_s *restrict p;
	elfin_item_s **section_array;
	uintptr_t section_count, i;
	refer_nstring_t str;
	uint64_t u64;
	uint32_t u32;
	p = r->inst;
	elfin_println("{%s}", r->name);
	if (p->func.get_ident_class(r, &u32))
		elfin_println("ident.class:      %u (%s)", u32, elfin_inner_get__ident_class(u32));
	if (p->func.get_ident_data(r, &u32))
		elfin_println("ident.data:       %u (%s)", u32, elfin_inner_get__ident_data(u32));
	if (p->func.get_ident_version(r, &u32))
		elfin_println("ident.version:    %u", u32);
	if (p->func.get_ident_osabi(r, &u32))
		elfin_println("ident.osabi:      %u (%s)", u32, elfin_inner_get__ident_osabi(u32));
	if (p->func.get_ident_abiversion(r, &u32))
		elfin_println("ident.abiversion: %u", u32);
	if (p->func.get_header_type(r, &u32))
		elfin_println("type:             %u (%s)", u32, elfin_inner_get__type(u32));
	if (p->func.get_header_machine(r, &u32))
		elfin_println("machine:          %u (%s)", u32, elfin_inner_get__machine(u32));
	if (p->func.get_header_version(r, &u32))
		elfin_println("version:          %u", u32);
	if (p->func.get_header_entry_address(r, &u64))
		elfin_println("entry-address:    0x%lx", u64);
	if (p->func.get_header_flags(r, &u32))
		elfin_println("flags:            %08x", u32);
	if (p->func.get_child_section_array(r, &section_array, &section_count))
	{
		for (i = 0; i < section_count; ++i)
		{
			if (!section_array[i]->inst->func.get_section_name(section_array[i], &str))
				str = NULL;
			elfin_println("section[%zu](%s):", i, str?str->string:s_empty);
			elfin_inner_dump_section(section_array[i], mlog, indent + elfin_dump_indent, r);
		}
	}
}

static void elfin_inner_dump(elfin_item_s *restrict item, mlog_s *mlog, uint32_t indent)
{
	switch (item->inst->item_id)
	{
		case elfin_item_id__elfin:
			elfin_inner_dump_elfin(item, mlog, indent);
			break;
		case elfin_item_id__section_null:
		case elfin_item_id__section_nobits:
		case elfin_item_id__section_program:
		case elfin_item_id__section_symbol:
		case elfin_item_id__section_string:
		case elfin_item_id__section_rela:
		case elfin_item_id__section_rel:
			elfin_inner_dump_section(item, mlog, indent, NULL);
			break;
		case elfin_item_id__entry_symbol:
			elfin_inner_dump_entry_symbol(item, mlog, indent, 0);
			break;
		case elfin_item_id__entry_rela:
		case elfin_item_id__entry_rel:
			elfin_inner_dump_entry_relo(item, mlog, indent, NULL, 0);
			break;
		default:
			break;
	}
}
