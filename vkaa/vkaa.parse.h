#ifndef _vkaa_parse_h_
#define _vkaa_parse_h_

#include "vkaa.h"
#include <hashmap.h>

typedef struct vkaa_parse_keyword_s vkaa_parse_keyword_s;
typedef struct vkaa_parse_operator_s vkaa_parse_operator_s;
typedef struct vkaa_parse_type2var_s vkaa_parse_type2var_s;

typedef enum vkaa_parse_rtype_t {
	vkaa_parse_rtype_none,
	vkaa_parse_rtype_var,
	vkaa_parse_rtype_selector,
	vkaa_parse_rtype_function,
} vkaa_parse_rtype_t;

typedef enum vkaa_parse_optype_t {
	vkaa_parse_optype_none,
	vkaa_parse_optype_unary_left,  // var[] var()
	vkaa_parse_optype_unary_right, // !var ~var
	vkaa_parse_optype_binary,      // var+var var.var
	vkaa_parse_optype_ternary,     // var?var:var
} vkaa_parse_optype_t;

typedef union vkaa_parse_rdata_t {
	refer_t none;
	vkaa_var_s *var;
	vkaa_selector_s *selector;
	vkaa_function_s *function;
} vkaa_parse_rdata_t;

typedef struct vkaa_parse_result_t {
	vkaa_parse_rtype_t type;
	vkaa_parse_rdata_t data;
} vkaa_parse_result_t;

typedef struct vkaa_parse_context_t {
	vkaa_execute_s *execute;
	vkaa_tpool_s *tpool;
	vkaa_scope_s *scope;
} vkaa_parse_context_t;

typedef struct vkaa_parse_syntax_t {
	const vkaa_syntax_t *syntax;
	uintptr_t number;
	uintptr_t pos;
} vkaa_parse_syntax_t;

typedef vkaa_parse_result_t* (*vkaa_parse_keyword_f)(const vkaa_parse_keyword_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, vkaa_parse_syntax_t *restrict syntax);
typedef vkaa_parse_result_t* (*vkaa_parse_operator_f)(const vkaa_parse_operator_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_parse_result_t *const param_list);
typedef vkaa_parse_result_t* (*vkaa_parse_type2var_f)(const vkaa_parse_type2var_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax);

struct vkaa_parse_s {
	hashmap_t keyword;
	hashmap_t operator;
	hashmap_t type2var;
};

struct vkaa_parse_keyword_s {
	vkaa_parse_keyword_f parse;
};

struct vkaa_parse_operator_s {
	vkaa_parse_operator_f parse;
	vkaa_parse_optype_t optype;
	const vkaa_oplevel_s *oplevel;
	const char *ternary_second_operator;
};

struct vkaa_parse_type2var_s {
	vkaa_parse_type2var_f parse;
};


#endif
