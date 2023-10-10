#ifndef _abc_adora_h_
#define _abc_adora_h_

#include <stdint.h>
#include <refer.h>

typedef enum abc_adora_vflag_t abc_adora_vflag_t;
typedef enum abc_adora_itype_t abc_adora_itype_t;
typedef union abc_adora_value_t abc_adora_value_t;
typedef struct abc_adora_vtype_t abc_adora_vtype_t;
typedef struct abc_adora_var_t abc_adora_var_t;
typedef struct abc_adora_symbol_t abc_adora_symbol_t;
typedef struct abc_adora_relocate_t abc_adora_relocate_t;
typedef struct abc_adora_import_setting_t abc_adora_import_setting_t;
typedef struct abc_adora_import_offset_t abc_adora_import_offset_t;
typedef struct abc_adora_icode_t abc_adora_icode_t;

typedef struct abc_adora_mcache_s abc_adora_mcache_s;
typedef struct abc_adora_symbol_s abc_adora_symbol_s;
typedef struct abc_adora_relocate_s abc_adora_relocate_s;
typedef struct abc_adora_import_s abc_adora_import_s;
typedef struct abc_adora_icode_s abc_adora_icode_s;

enum abc_adora_vflag_t {
	abc_adora_vflag__read  = 0x01,
	abc_adora_vflag__write = 0x02,
	abc_adora_vflag__xcall = 0x04,
};

enum abc_adora_itype_t {
	abc_adora_itype__label,
	abc_adora_itype__instruction,
	abc_adora_itype_max
};

union abc_adora_value_t {
	uint64_t u64;
	int64_t s64;
	double f64;
	uint32_t u32;
	int32_t s32;
	float f32;
};

struct abc_adora_vtype_t {
	uint32_t vtype;
	uint32_t vflag;
};

struct abc_adora_var_t {
	abc_adora_vtype_t vtype;
	abc_adora_value_t value;
};

struct abc_adora_symbol_t {
	refer_nstring_t segment;
	refer_nstring_t symbol;
	uint32_t segment_index;
	uint32_t symbol_index;
	uint32_t symbol_per_segment;
	uint32_t symbol_type;
};

struct abc_adora_relocate_t {
	uint32_t source_symbol;
	uint32_t relocate_type;
	int64_t relocate_addend;
	uint64_t target_offset;
};

struct abc_adora_import_setting_t {
	uint32_t import_type;
	uint32_t segment_index;
	uint64_t symbol_offset;
};

struct abc_adora_import_offset_t {
	uint64_t segment_offset;
	uint64_t symbol_offset;
};

struct abc_adora_icode_t {
	abc_adora_var_t icode;
	uintptr_t var_offset;
	uintptr_t var_number;
};

// mcache

abc_adora_mcache_s* abc_adora_mcache_alloc(void);
abc_adora_mcache_s* abc_adora_mcache_append(abc_adora_mcache_s *restrict m, const void *restrict data, uintptr_t size);
abc_adora_mcache_s* abc_adora_mcache_fillch(abc_adora_mcache_s *restrict m, int32_t ch, uintptr_t count);
uintptr_t abc_adora_mcache_offset(const abc_adora_mcache_s *restrict m);
abc_adora_mcache_s* abc_adora_mcache_restore(abc_adora_mcache_s *restrict m, uintptr_t offset);
uint8_t* abc_adora_mcache_mapping(abc_adora_mcache_s *restrict m, uintptr_t *restrict size);
void abc_adora_mcache_clear(abc_adora_mcache_s *restrict m);

// symbol

abc_adora_symbol_s* abc_adora_symbol_alloc(void);
void abc_adora_symbol_clear(abc_adora_symbol_s *restrict s);
abc_adora_symbol_s* abc_adora_symbol_touch_segment(abc_adora_symbol_s *restrict s, const char *restrict segment);
const abc_adora_symbol_t* abc_adora_symbol_create_nstring(abc_adora_symbol_s *restrict s, const char *restrict segment, refer_nstring_t symbol, uint32_t symbol_type);
const abc_adora_symbol_t* abc_adora_symbol_create_cstring(abc_adora_symbol_s *restrict s, const char *restrict segment, const char *restrict symbol, uint32_t symbol_type);
const abc_adora_symbol_s* abc_adora_symbol_find_segment(const abc_adora_symbol_s *restrict s, const char *restrict segment, uint32_t *restrict segment_index, uint32_t *restrict symbol_count);
const abc_adora_symbol_t* abc_adora_symbol_find_symbol(abc_adora_symbol_s *restrict s, const char *restrict segment, const char *restrict symbol);
uintptr_t abc_adora_symbol_count_segment(abc_adora_symbol_s *restrict s);
uintptr_t abc_adora_symbol_count_symbol(abc_adora_symbol_s *restrict s);

// relocate

abc_adora_relocate_s* abc_adora_relocate_alloc(void);
void abc_adora_relocate_clear(abc_adora_relocate_s *restrict r);
const abc_adora_relocate_t* abc_adora_relocate_append(abc_adora_relocate_s *restrict r, uint32_t source_symbol, uint32_t relocate_type, int64_t relocate_addend, uint64_t target_offset);
const abc_adora_relocate_t* abc_adora_relocate_mapping(abc_adora_relocate_s *restrict r, uintptr_t *restrict relocate_count);

// import

abc_adora_import_s* abc_adora_import_alloc(uint32_t import_type_number);
void abc_adora_import_clear(abc_adora_import_s *restrict i);
abc_adora_import_s* abc_adora_import_create_segment(abc_adora_import_s *restrict i, uint32_t segment_index, uint64_t segment_offset);
abc_adora_import_s* abc_adora_import_create_import(abc_adora_import_s *restrict i, uint32_t import_index, uintptr_t n, const abc_adora_import_setting_t setting[]);
const abc_adora_import_s* abc_adora_import_get_offset(const abc_adora_import_s *restrict i, uint32_t import_index, uintptr_t n, const uint32_t import_type[], abc_adora_import_offset_t offset[]);

#endif
