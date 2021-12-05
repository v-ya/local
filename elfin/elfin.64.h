#ifndef _elfin_64_h_
#define _elfin_64_h_

#include "elfin.def.h"

struct elfin_64_header_t {
	struct elfin_indent_t indent;
	uint16_t type;                         // Object file type                  (enum elfin_type_t)
	uint16_t machine;                      // Architecture                      (enum elfin_machine_t)
	uint32_t version;                      // Object file version               (enum elfin_version_t)
	uint64_t addr_entry;                   // Entry point virtual address
	uint64_t offset_program_header;        // Program header table file offset
	uint64_t offset_section_header;        // Section header table file offset
	uint32_t flags;                        // Processor-specific flags
	uint16_t elf_header_size;              // ELF header size in bytes
	uint16_t program_header_entry_size;    // Program header table entry size
	uint16_t program_header_entry_size;    // Program header table entry count
	uint16_t section_header_entry_size;    // Section header table entry size
	uint16_t section_header_entry_size;    // Section header table entry count
	uint16_t section_header_string_index;  // Section header string table index
};

struct elfin_64_section_header_t {
	uint32_t name;       // Section name (string tbl index)
	uint32_t type;       // Section type                       (enum elfin_section_type_t)
	uint64_t flags;      // Section flags                      (enum elfin_section_flag_t)
	uint64_t addr;       // Section virtual addr at execution
	uint64_t offset;     // Section file offset
	uint64_t size;       // Section size in bytes
	uint32_t link;       // Link to another section
	uint32_t info;       // Additional section information
	uint64_t addr_align; // Section alignment
	uint64_t ebtry_size; // Entry size if section holds table
};

// type == elfin_section_type__symbol_table
struct elfin_64_symbol_entry_t {
	uint32_t name;          // Symbol name (string tbl index)
	uint8_t info;           // Symbol type and binding         (enum elfin_symbol_bind_t) (enum elfin_symbol_type_t)
	uint8_t other;          // Symbol visibility
	uint16_t section_index; // Section index
	uint64_t addr;          // Symbol value
	uint64_t size;          // Symbol size
};

// type == elfin_section_type__rel
struct elfin_64_rel_entry_t {
	uint64_t offset; // Address
	uint64_t info;   // Relocation type and symbol index
};

// type == elfin_section_type__rela
struct elfin_64_rela_entry_t {
	struct elfin_64_rel_entry_t rel;
	int64_t addend;  // Addend
};

static inline uint8_t elfin_64_symbol_entry__get_type(const struct elfin_64_symbol_entry_t *restrict e) { return e->info & 0x0f; }
static inline uint8_t elfin_64_symbol_entry__get_bind(const struct elfin_64_symbol_entry_t *restrict e) { return e->info >> 4; }
static inline void elfin_64_symbol_entry__set_type(struct elfin_64_symbol_entry_t *restrict e, uint8_t type) { e->info = (e->info & 0xf0) | (type & 0x0f); }
static inline void elfin_64_symbol_entry__set_bind(struct elfin_64_symbol_entry_t *restrict e, uint8_t bind) { e->info = (e->info & 0x0f) | (uint8_t) (bind << 4); }

static inline uint32_t elfin_64_rel_entry__get_symbol(const struct elfin_64_rel_entry_t *restrict e) { return (uint32_t) (e->info >> 32); }
static inline uint32_t elfin_64_rel_entry__get_type(const struct elfin_64_rel_entry_t *restrict e) { return (uint32_t) (e->info & 0xffffffff); }
static inline void elfin_64_rel_entry__set_symbol(struct elfin_64_rel_entry_t *restrict e, uint32_t symbol) { e->info = (e->info & 0xffffffff) | ((uint64_t) symbol << 32); }
static inline void elfin_64_rel_entry__set_type(struct elfin_64_rel_entry_t *restrict e, uint32_t type) { e->info = (e->info & (uint64_t) 0xffffffff00000000) | type; }

#endif
