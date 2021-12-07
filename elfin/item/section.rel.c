#include "section.h"
#include "../inner.inst.h"
#include "../elfin.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_rel(void)
{
	return elfin_inner_alloc_item_inst_section_entry_array(
		elfin_name_section_rel,
		elfin_item_id__entry_rel,
		elfin_section_type__rel,
		elfin_section_flag__info_link, 8, 24
	);
}
