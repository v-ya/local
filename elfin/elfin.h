#ifndef _elfin_h_
#define _elfin_h_

#include <refer.h>

typedef struct elfin_inst_s elfin_inst_s;
typedef struct elfin_item_s elfin_item_s;

typedef enum elfin_item_id_t {
	elfin_item_id__elfin,
	elfin_item_id__section_null,
	elfin_item_id__section_nobits,
	elfin_item_id__section_program,
	elfin_item_id__section_symbol,
	elfin_item_id__section_string,
	elfin_item_id__section_rela,
	elfin_item_id__section_rel,
	elfin_item_id__entry_symbol,
	elfin_item_id__entry_rela,
	elfin_item_id__entry_rel,
	elfin_item_id_number
} elfin_item_id_t;

#endif
