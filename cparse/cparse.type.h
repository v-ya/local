#ifndef _cparse_type_h_
#define _cparse_type_h_

#include "cparse.h"
#include <tparse/tword.h>
#include <tparse/tmapping.h>
#include <tparse/tstring.h>
#include <tparse/tstack.h>

typedef struct cparse_inst_context_t cparse_inst_context_t;
typedef struct cparse_parse_s cparse_parse_s;

typedef cparse_parse_s* (*cparse_parse_f)(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context);

struct cparse_inst_s {
	tparse_tstack_s *stack;
	tparse_tmapping_s *parse;
};

struct cparse_inst_context_t {
	vattr_s *scope;
	const char *restrict data;
	uintptr_t size;
	uintptr_t pos;
};

struct cparse_parse_s {
	cparse_parse_f parse;
};

struct cparse_value_s* cparse_inner_alloc_value_empty(cparse_value_type_t type);
struct cparse_value_s* cparse_inner_alloc_value_string(cparse_value_type_t type, const char *restrict string, uintptr_t length);
struct cparse_value_s* cparse_inner_alloc_value_nstring(cparse_value_type_t type, const char *restrict string, uintptr_t length);

cparse_parse_s* cparse_inner_alloc_parse_key(void);
cparse_parse_s* cparse_inner_alloc_parse_string(void);
cparse_parse_s* cparse_inner_alloc_parse_chars(void);

tparse_tmapping_s* cparse_inner_alloc_tmapping_parse(void);
tparse_tstring_s* cparse_inner_alloc_tstring_number(void);
tparse_tword_edge_s* cparse_inner_alloc_tword_key(void);

cparse_inst_s* cparse_inner_scope_parse(cparse_inst_s *restrict inst, vattr_s *restrict scope, const char *restrict data, uintptr_t size);

#endif
