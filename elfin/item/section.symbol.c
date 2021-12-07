#include "section.h"
#include "../inner.inst.h"
#include "../elfin.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_symbol(void)
{
	return elfin_inner_alloc_item_inst_section_entry_array(
		elfin_name_section_symbol,
		elfin_item_id__entry_symbol,
		elfin_section_type__symbol_table,
		0, 8, 24
	);
}
