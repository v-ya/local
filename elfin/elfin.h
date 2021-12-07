#ifndef _elfin_h_
#define _elfin_h_

#include <refer.h>

typedef struct elfin_inst_s elfin_inst_s;
typedef struct elfin_item_s elfin_item_s;

elfin_inst_s* elfin_inst_alloc(void);
elfin_item_s* elfin_create_elfin(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_section_null(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_section_nobits(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_section_program(const elfin_inst_s *restrict inst);

#define elfin_type_elfin           "elfin"
#define elfin_name_elfin           "elfin"
#define elfin_type_section         "section"
#define elfin_name_section_null    "section.null"
#define elfin_name_section_nobits  "section.nobits"
#define elfin_name_section_program "section.program"

const char* elfin_item_name(const elfin_item_s *restrict item);
const char* elfin_item_type(const elfin_item_s *restrict item);

elfin_item_s* elfin_load_elfin_by_memory(const elfin_inst_s *restrict inst, const uint8_t *restrict data, uintptr_t size);
elfin_item_s* elfin_load_elfin_by_path(const elfin_inst_s *restrict inst, const char *restrict path);

#endif
