#ifndef _vkaa_std_extern_h_
#define _vkaa_std_extern_h_

#include "vkaa.h"
#include "vkaa.std.h"
#include "vkaa.syntax.h"
#include "vkaa.oplevel.h"
#include "vkaa.tpool.h"
#include "vkaa.type.h"
#include "vkaa.var.h"
#include "vkaa.scope.h"
#include "vkaa.selector.h"
#include "vkaa.function.h"
#include "vkaa.execute.h"
#include "vkaa.parse.h"

#define vkaa_std_oplevel_assign    "assign"
#define vkaa_std_oplevel_condition "condition"
#define vkaa_std_oplevel_relation  "relation"
#define vkaa_std_oplevel_logic     "logic"
#define vkaa_std_oplevel_bitwise   "bitwise"
#define vkaa_std_oplevel_arith_1   "arith.1"
#define vkaa_std_oplevel_arith_2   "arith.2"
#define vkaa_std_oplevel_arith_3   "arith.3"
#define vkaa_std_oplevel_unary     "unary"
#define vkaa_std_oplevel_inquiry   "inquiry"

// var<void>

typedef struct vkaa_var_s vkaa_std_var_void_s;

// var<scope>

typedef struct vkaa_std_var_scope_s {
	vkaa_var_s var;
	vkaa_scope_s *scope;
} vkaa_std_var_scope_s;

// var<syntax>

typedef struct vkaa_std_var_syntax_s {
	vkaa_var_s var;
	vkaa_syntax_type_t type;
	vkaa_syntax_s *syntax;
} vkaa_std_var_syntax_s;

// var<function>

typedef struct vkaa_std_var_function_param_s {
	uintptr_t number;
	vkaa_var_s *param[];
} vkaa_std_var_function_param_s;

typedef struct vkaa_std_var_function_s {
	vkaa_var_s var;
	vkaa_scope_s *scope;
	vkaa_execute_s *exec;
	vkaa_std_var_function_param_s *param;
} vkaa_std_var_function_s;

// var<string>

typedef struct vkaa_std_var_string_s {
	vkaa_var_s var;
	refer_nstring_t value;
} vkaa_std_var_string_s;

// var<uint>

typedef struct vkaa_std_var_uint_s {
	vkaa_var_s var;
	uintptr_t value;
} vkaa_std_var_uint_s;

// var<int>

typedef struct vkaa_std_var_int_s {
	vkaa_var_s var;
	intptr_t value;
} vkaa_std_var_int_s;

// var<float>

typedef struct vkaa_std_var_float_s {
	vkaa_var_s var;
	double value;
} vkaa_std_var_float_s;

// tpool

vkaa_type_s* vkaa_std_tpool_set(vkaa_tpool_s *restrict tpool, const char *restrict name, uintptr_t id, vkaa_type_create_f create);
vkaa_type_s* vkaa_std_tpool_set_with_data(vkaa_tpool_s *restrict tpool, const char *restrict name, uintptr_t id, vkaa_type_create_f create, uintptr_t size);

// parse

typedef struct vkaa_std_type2var_with_id_s {
	vkaa_parse_type2var_s type2var;
	uintptr_t id_number;
	uintptr_t id[];
} vkaa_std_type2var_with_id_s;

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword(vkaa_parse_s *restrict p, const char *restrict keyword, vkaa_parse_keyword_f parse, vkaa_parse_keytype_t keytype);
vkaa_parse_operator_s* vkaa_std_parse_set_operator(vkaa_parse_s *restrict p, const vkaa_oplevel_s *restrict opl, const char *restrict operator, vkaa_parse_operator_f parse, vkaa_parse_optype_t optype, const char *restrict oplevel, const char *restrict ternary_second_operator);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var(vkaa_parse_s *restrict p, vkaa_syntax_type_t type, vkaa_parse_type2var_f parse, vkaa_parse_type2var_type_t type2var_type);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_with_id(vkaa_parse_s *restrict p, vkaa_syntax_type_t type, vkaa_parse_type2var_f parse, vkaa_parse_type2var_type_t type2var_type, const uintptr_t id[], uintptr_t id_number);

#endif
