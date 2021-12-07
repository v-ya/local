#ifndef _elfin_inner_item_section_h_
#define _elfin_inner_item_section_h_

#include "../inner.item.h"

struct elfin_item_section_s {
	struct elfin_item_s item;
	refer_nstring_t name;
	refer_nstring_t link;
	refer_nstring_t info;
	uint32_t type;
	uint64_t flags;
	uint64_t exec_addr;
	uint64_t size;
	uint64_t alignment;
	uint64_t entry_size;
};

void elfin_inner_clear_item_section(struct elfin_item_section_s *restrict section);
struct elfin_item_s* elfin_inner_link_item_section(struct elfin_item_section_s *restrict section, const struct elfin_item_inst_s *restrict inst);
void elfin_inner_init_item_section_inst(struct elfin_item_inst_s *restrict inst);

struct elfin_item_inst_s* elfin_inner_alloc_item_inst_section_entry_array(const char *restrict name, uintptr_t allow_item_id, uint32_t default_type, uint64_t default_flags, uint64_t default_alignment, uint64_t default_entry_size);

#endif
