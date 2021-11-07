#ifndef _cparse_type_h_
#define _cparse_type_h_

#include "cparse.h"
#include <tparse/tword.h>
#include <tparse/tmapping.h>
#include <tparse/tstring.h>
#include <tparse/tstack.h>

typedef struct cparse_inst_context_t cparse_inst_context_t;
typedef struct cparse_parse_s cparse_parse_s;
typedef struct cparse_stack_scope_s cparse_stack_scope_s;

typedef cparse_parse_s* (*cparse_parse_f)(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context);

struct cparse_inst_s {
	tparse_tstack_s *stack;
	tparse_tmapping_s *parse;
};

struct cparse_inst_context_t {
	vattr_s *scope;
	refer_nstring_t code;
	const char *data;
	uintptr_t size;
	uintptr_t pos;
};

struct cparse_parse_s {
	cparse_parse_f parse;
};

struct cparse_stack_scope_s {
	vattr_s *inner_scope_last;
	cparse_scope_s *scope_curr;
};

cparse_scope_s* cparse_inner_alloc_scope_empty(void);

struct cparse_value_s* cparse_inner_alloc_value_empty(cparse_value_type_t type);
struct cparse_value_s* cparse_inner_alloc_value_nstring(cparse_value_type_t type, const char *restrict string, uintptr_t length);

cparse_parse_s* cparse_inner_parse_comment_line(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context);
cparse_parse_s* cparse_inner_parse_comment_block(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context);
cparse_parse_s* cparse_inner_parse_scope_into(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context);
cparse_parse_s* cparse_inner_parse_scope_out(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context);

cparse_parse_s* cparse_inner_alloc_parse_key(void);
cparse_parse_s* cparse_inner_alloc_parse_string(void);
cparse_parse_s* cparse_inner_alloc_parse_chars(void);
cparse_parse_s* cparse_inner_alloc_parse_number(void);
cparse_parse_s* cparse_inner_alloc_parse_operator(void);
cparse_parse_s* cparse_inner_alloc_parse_pre(void);

tparse_tmapping_s* cparse_inner_alloc_tmapping_parse(void);
tparse_tstring_s* cparse_inner_alloc_tstring_number(void);
tparse_tword_edge_s* cparse_inner_alloc_tword_key(void);

cparse_inst_s* cparse_inner_scope_parse(cparse_inst_s *restrict inst, cparse_scope_s *restrict scope);

#endif
