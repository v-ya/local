#ifndef _abc_adora_adora_h_
#define _abc_adora_adora_h_

#include "../adora.h"

// adora.iset.c

typedef struct abc_adora_iset_s abc_adora_iset_s;

struct abc_adora_iset_s {
	uint64_t iset_flag;
	refer_nstring_t iset_name;
};

abc_adora_iset_s* abc_adora_iset_alloc(const char *restrict iset_name, uint64_t iset_flag);

// adora.instr.c

typedef struct abc_adora_instr_s abc_adora_instr_s;

struct abc_adora_instr_s {
	refer_nstring_t instr_name;
	uintptr_t instr_index;
	abc_adora_instr_t instr;
};

abc_adora_instr_s* abc_adora_instr_alloc(refer_nstring_t instr_name, uintptr_t instr_index, const abc_adora_instr_t *restrict instr);
const abc_adora_instr_s* abc_adora_instr_match(const abc_adora_instr_s *restrict instr, uintptr_t vtype_count, const abc_adora_vtype_t *restrict vtype_array);

// adora.ilink.c

typedef struct abc_adora_ilink_s abc_adora_ilink_s;
typedef struct abc_adora_ilink_t abc_adora_ilink_t;

struct abc_adora_ilink_t {
	uint64_t exist_iset_flags;
	refer_nstring_t instr_name;
};

abc_adora_ilink_s* abc_adora_ilink_alloc(void);
abc_adora_ilink_s* abc_adora_ilink_append(abc_adora_ilink_s *restrict r, refer_nstring_t instr_name, uint64_t exist_iset_flags);
const abc_adora_ilink_t* abc_adora_ilink_mapping(const abc_adora_ilink_s *restrict r, uintptr_t *restrict ilink_count);

// adora.isa.c

const abc_adora_instr_s* abc_adora_isa_match_instr(const abc_adora_isa_s *restrict isa, uint64_t iset_flags, uint64_t instr_id, uintptr_t vtype_count, const abc_adora_vtype_t *restrict vtype_array);

#endif
