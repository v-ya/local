#ifndef _vkaa_parse_h_
#define _vkaa_parse_h_

#include "vkaa.h"
#include <hashmap.h>
#include <tparse/tstack.h>

#define vkaa_parse_operator_brackets  "()"
#define vkaa_parse_operator_square    "[]"

typedef struct vkaa_parse_keyword_s vkaa_parse_keyword_s;
typedef struct vkaa_parse_operator_s vkaa_parse_operator_s;
typedef struct vkaa_parse_type2var_s vkaa_parse_type2var_s;

typedef enum vkaa_parse_rtype_t {
	vkaa_parse_rtype_none,
	vkaa_parse_rtype_var,
	vkaa_parse_rtype_selector,
	vkaa_parse_rtype_function,
} vkaa_parse_rtype_t;

typedef enum vkaa_parse_keytype_t {
	vkaa_parse_keytype_complete,
	vkaa_parse_keytype_inner,
} vkaa_parse_keytype_t;

typedef enum vkaa_parse_optype_t {
	vkaa_parse_optype_none,
	vkaa_parse_optype_unary_left,             // var[] var()
	vkaa_parse_optype_unary_right,            // !var ~var -var
	vkaa_parse_optype_binary,                 // var+var var*var
	vkaa_parse_optype_binary_second_type2var, // var.name => var["name"]
	vkaa_parse_optype_ternary_first,          // var?var:var
	vkaa_parse_optype_ternary_second,         // var?var:var
} vkaa_parse_optype_t;

typedef enum vkaa_parse_optowards_t {
	vkaa_parse_optowards_left2right,
	vkaa_parse_optowards_right2left,
} vkaa_parse_optowards_t;

typedef enum vkaa_parse_type2var_type_t {
	vkaa_parse_keytype_normal,
	vkaa_parse_keytype_first_allow_ignore,
} vkaa_parse_type2var_type_t;

typedef union vkaa_parse_rdata_t {
	refer_t none;
	vkaa_var_s *var;
	vkaa_selector_s *selector;
	vkaa_function_s *function;
} vkaa_parse_rdata_t;

typedef struct vkaa_parse_result_t {
	vkaa_parse_rtype_t type;
	vkaa_parse_rdata_t data;
	vkaa_var_s *this;
} vkaa_parse_result_t;

typedef struct vkaa_parse_context_t {
	vkaa_parse_s *parse;
	tparse_tstack_s *stack;
	vkaa_execute_s *execute;
	vkaa_tpool_s *tpool;
	vkaa_scope_s *scope;
	vkaa_var_s *this;
	const vkaa_syntax_source_s *source;
} vkaa_parse_context_t;

typedef struct vkaa_parse_syntax_t {
	const vkaa_syntax_t *syntax;
	uintptr_t number;
	uintptr_t pos;
} vkaa_parse_syntax_t;

typedef vkaa_parse_result_t* (*vkaa_parse_keyword_f)(const vkaa_parse_keyword_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, vkaa_parse_syntax_t *restrict syntax);
typedef vkaa_parse_result_t* (*vkaa_parse_operator_f)(const vkaa_parse_operator_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax, const vkaa_parse_result_t *const param_list);
typedef vkaa_parse_result_t* (*vkaa_parse_type2var_f)(const vkaa_parse_type2var_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax);

struct vkaa_parse_s {
	hashmap_t keyword;
	hashmap_t operator; // (other)
	hashmap_t op1unary; // vkaa_parse_optype_unary_right
	hashmap_t type2var;
};

struct vkaa_parse_keyword_s {
	vkaa_parse_keyword_f parse;
	vkaa_parse_keytype_t keytype;
};

struct vkaa_parse_operator_s {
	vkaa_parse_operator_f parse;
	vkaa_parse_operator_f op_left_okay_notify;
	vkaa_parse_optype_t optype;
	vkaa_parse_optowards_t towards;
	const vkaa_level_s *oplevel;
	const char *ternary_second_operator;
};

struct vkaa_parse_type2var_s {
	vkaa_parse_type2var_f parse;
	vkaa_parse_type2var_type_t type;
};

vkaa_parse_s* vkaa_parse_alloc(void);
vkaa_parse_keyword_s* vkaa_parse_keyword_get(const vkaa_parse_s *restrict parse, const char *restrict keyword);
vkaa_parse_keyword_s* vkaa_parse_keyword_set(vkaa_parse_s *restrict parse, const char *restrict keyword, vkaa_parse_keyword_s *restrict kw);
void vkaa_parse_keyword_unset(vkaa_parse_s *restrict parse, const char *restrict keyword);
vkaa_parse_operator_s* vkaa_parse_operator_get(const vkaa_parse_s *restrict parse, const char *restrict operator);
vkaa_parse_operator_s* vkaa_parse_operator_set(vkaa_parse_s *restrict parse, const char *restrict operator, vkaa_parse_operator_s *restrict op);
void vkaa_parse_operator_unset(vkaa_parse_s *restrict parse, const char *restrict operator);
vkaa_parse_operator_s* vkaa_parse_op1unary_get(const vkaa_parse_s *restrict parse, const char *restrict operator);
vkaa_parse_operator_s* vkaa_parse_op1unary_set(vkaa_parse_s *restrict parse, const char *restrict operator, vkaa_parse_operator_s *restrict op);
void vkaa_parse_op1unary_unset(vkaa_parse_s *restrict parse, const char *restrict operator);
vkaa_parse_type2var_s* vkaa_parse_type2var_get(const vkaa_parse_s *restrict parse, vkaa_syntax_type_t type);
vkaa_parse_type2var_s* vkaa_parse_type2var_set(vkaa_parse_s *restrict parse, vkaa_syntax_type_t type, vkaa_parse_type2var_s *restrict t2v);
void vkaa_parse_type2var_unset(vkaa_parse_s *restrict parse, vkaa_syntax_type_t type);

const vkaa_syntax_t* vkaa_parse_syntax_fetch_and_next(vkaa_parse_syntax_t *restrict syntax);
const vkaa_syntax_t* vkaa_parse_syntax_next_and_fetch(vkaa_parse_syntax_t *restrict syntax);
const vkaa_syntax_t* vkaa_parse_syntax_get_curr(vkaa_parse_syntax_t *restrict syntax);
const vkaa_syntax_t* vkaa_parse_syntax_get_last(vkaa_parse_syntax_t *restrict syntax);
const vkaa_syntax_t* vkaa_parse_syntax_get_range(vkaa_parse_syntax_t *restrict syntax, uintptr_t *restrict number, vkaa_syntax_type_t tail_type, const char *restrict tail_string);

void vkaa_parse_operator_finally(vkaa_parse_operator_s *restrict operator);

void vkaa_parse_result_initial(vkaa_parse_result_t *restrict result);
void vkaa_parse_result_clear(vkaa_parse_result_t *restrict result);
void vkaa_parse_result_set(vkaa_parse_result_t *restrict dst, const vkaa_parse_result_t *restrict src);
vkaa_var_s* vkaa_parse_result_get_var(const vkaa_parse_result_t *restrict result);
const vkaa_type_s* vkaa_parse_result_get_type(const vkaa_parse_result_t *restrict result);

const vkaa_parse_context_t* vkaa_parse_parse(const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax, uintptr_t number, vkaa_parse_result_t *restrict result);

#endif
