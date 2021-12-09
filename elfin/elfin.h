#ifndef _elfin_h_
#define _elfin_h_

#include <refer.h>
#include <mlog.h>

typedef struct elfin_inst_s elfin_inst_s;
typedef struct elfin_item_s elfin_item_s;

elfin_inst_s* elfin_inst_alloc(void);
elfin_item_s* elfin_create_elfin(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_section_null(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_section_nobits(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_section_program(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_section_symbol(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_section_string(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_section_rela(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_section_rel(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_entry_symbol(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_entry_rela(const elfin_inst_s *restrict inst);
elfin_item_s* elfin_create_entry_rel(const elfin_inst_s *restrict inst);

#define elfin_type_elfin           "elfin"
#define elfin_name_elfin           "elfin"
#define elfin_type_section         "section"
#define elfin_name_section_null    "section.null"
#define elfin_name_section_nobits  "section.nobits"
#define elfin_name_section_program "section.program"
#define elfin_name_section_symbol  "section.symbol"
#define elfin_name_section_string  "section.string"
#define elfin_name_section_rela    "section.rela"
#define elfin_name_section_rel     "section.rel"
#define elfin_type_entry           "entry"
#define elfin_name_entry_symbol    "entry.symbol"
#define elfin_name_entry_rela      "entry.rela"
#define elfin_name_entry_rel       "entry.rel"

const char* elfin_item_name(const elfin_item_s *restrict item);
const char* elfin_item_type(const elfin_item_s *restrict item);

elfin_item_s* elfin_load_elfin_by_memory(const elfin_inst_s *restrict inst, const void *restrict data, uintptr_t size);
elfin_item_s* elfin_load_elfin_by_path(const elfin_inst_s *restrict inst, const char *restrict path);

void elfin_dump(elfin_item_s *restrict item, mlog_s *mlog);

#endif
