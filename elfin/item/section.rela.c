#include "section.h"
#include "../inner.inst.h"
#include "../elfin.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_rela(void)
{
	return elfin_inner_alloc_item_inst_section_entry_array(
		elfin_name_section_rela,
		elfin_item_id__entry_rela,
		elfin_section_type__rela,
		elfin_section_flag__info_link, 8, 24
	);
}
