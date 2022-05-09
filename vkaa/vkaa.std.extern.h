#ifndef _vkaa_std_extern_h_
#define _vkaa_std_extern_h_

#include "vkaa.h"
#include "vkaa.std.h"
#include "vkaa.syntax.h"
#include "vkaa.oplevel.h"
#include "vkaa.error.h"
#include "vkaa.tpool.h"
#include "vkaa.type.h"
#include "vkaa.var.h"
#include "vkaa.scope.h"
#include "vkaa.selector.h"
#include "vkaa.function.h"
#include "vkaa.execute.h"
#include "vkaa.parse.h"

// oplevel

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

// op suffix

#define vkaa_std_op_testeval_suffix "pre"

// control preset label

#define vkaa_std_label_stack     ".stack"
#define vkaa_std_label_break     ".break"
#define vkaa_std_label_continue  ".continue"
#define vkaa_std_label_if_next   ".if.next"
#define vkaa_std_label_if_end    ".if.end"

// error

#define vkaa_std_error_memory_less "vkaa.std.memory_less"
#define vkaa_std_error_div_zero    "vkaa.std.div_zero"

// var<void>

typedef struct vkaa_var_s vkaa_std_var_void_s;

// var<null>

typedef struct vkaa_var_s vkaa_std_var_null_s;

// var<scope>

typedef struct vkaa_std_var_scope_s {
	vkaa_var_s var;
	vkaa_scope_s *scope;
} vkaa_std_var_scope_s;

vkaa_std_var_scope_s* vkaa_std_type_scope_create_by_parent(const vkaa_type_s *restrict type, vkaa_scope_s *restrict scope);

// var<syntax>

typedef struct vkaa_std_var_syntax_s {
	vkaa_var_s var;
	vkaa_syntax_type_t type;
	const vkaa_syntax_s *syntax;
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

// var<bool>

typedef uintptr_t vkaa_std_bool_t;

typedef struct vkaa_std_var_bool_s {
	vkaa_var_s var;
	vkaa_std_bool_t value;
} vkaa_std_var_bool_s;

// var<string>

typedef struct vkaa_std_var_string_s {
	vkaa_var_s var;
	refer_nstring_t value;
} vkaa_std_var_string_s;

// var<uint>

typedef uintptr_t vkaa_std_uint_t;

typedef struct vkaa_std_var_uint_s {
	vkaa_var_s var;
	vkaa_std_uint_t value;
} vkaa_std_var_uint_s;

// var<int>

typedef intptr_t vkaa_std_int_t;

typedef struct vkaa_std_var_int_s {
	vkaa_var_s var;
	vkaa_std_int_t value;
} vkaa_std_var_int_s;

// var<float>

typedef double vkaa_std_float_t;

typedef struct vkaa_std_var_float_s {
	vkaa_var_s var;
	vkaa_std_float_t value;
} vkaa_std_var_float_s;

// tpool

typedef vkaa_type_s* (*vkaa_std_type_initial_f)(vkaa_type_s *restrict type, vkaa_std_typeid_s *restrict typeid);

vkaa_type_s* vkaa_std_tpool_set(vkaa_tpool_s *restrict tpool, const char *restrict name, uintptr_t id, vkaa_type_create_f create, vkaa_std_type_initial_f initial, vkaa_std_typeid_s *restrict typeid);

// convert

const vkaa_selector_s* vkaa_std_convert_test_by_type(const vkaa_type_s *restrict src, const vkaa_type_s *restrict dst);
const vkaa_selector_s* vkaa_std_convert_test_by_name(const vkaa_type_s *restrict src, const char *restrict dst);
const vkaa_selector_s* vkaa_std_convert_test_by_typeid(const vkaa_type_s *restrict src, uintptr_t dst, const vkaa_tpool_s *restrict tpool);
vkaa_var_s* vkaa_std_convert_by_var(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_var_s *restrict src, vkaa_var_s *restrict dst);
vkaa_function_s* vkaa_std_convert_by_typeid(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_var_s *restrict src, uintptr_t dst);
vkaa_var_s* vkaa_std_convert_result_get_var(const vkaa_parse_result_t *restrict result, vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, uintptr_t type_id);

// selector

typedef enum vkaa_std_selector_output_t {
	vkaa_std_selector_output_any,
	vkaa_std_selector_output_this,
	vkaa_std_selector_output_input_first,
	vkaa_std_selector_output_new,
} vkaa_std_selector_output_t;

typedef enum vkaa_std_selector_convert_t {
	vkaa_std_selector_convert_none,
	vkaa_std_selector_convert_promotion,
} vkaa_std_selector_convert_t;

typedef struct vkaa_std_selector_desc_t {
	vkaa_function_f function;
	vkaa_std_selector_output_t output;
	vkaa_std_selector_convert_t convert;
	uintptr_t this_typeid;
	uintptr_t output_typeid;
	uintptr_t input_number;
	uintptr_t input_typeid[];
} vkaa_std_selector_desc_t;

typedef struct vkaa_std_selector_s {
	vkaa_selector_s selector;
	vattr_s *std_desc;
} vkaa_std_selector_s;

vkaa_std_selector_s* vkaa_std_selector_initial(vkaa_std_selector_s *restrict selector);
void vkaa_std_selector_finally(vkaa_std_selector_s *restrict selector);
vkaa_std_selector_s* vkaa_std_selector_alloc(void);
vkaa_std_selector_s* vkaa_std_selector_append(vkaa_std_selector_s *restrict selector, const char *restrict name, vkaa_function_f function, vkaa_std_selector_output_t output, vkaa_std_selector_convert_t convert, uintptr_t this_typeid, uintptr_t output_typeid, uintptr_t input_number, const uintptr_t *restrict input_typeid);

const vkaa_std_selector_desc_t* vkaa_std_selector_test(const vkaa_std_selector_desc_t *restrict desc, const vkaa_selector_param_t *restrict param, uintptr_t *restrict score);
vkaa_function_s* vkaa_std_selector_create(const vkaa_std_selector_s *restrict selector, const vkaa_selector_param_t *restrict param, const vkaa_std_selector_desc_t *restrict desc);
vkaa_function_s* vkaa_std_selector_selector(const vkaa_std_selector_s *restrict selector, const vkaa_selector_param_t *restrict param);

// parse

typedef struct vkaa_std_op_name_t {
	char op_name[16];
} vkaa_std_op_name_t;

const char* vkaa_std_op_testeval_name(vkaa_std_op_name_t *restrict cache, const char *restrict op);

typedef struct vkaa_std_keyword_s {
	vkaa_parse_keyword_s keyword;
	vkaa_std_typeid_s *typeid;
} vkaa_std_keyword_s;

typedef struct vkaa_std_operator_s {
	vkaa_parse_operator_s operator;
	vkaa_std_typeid_s *typeid;
} vkaa_std_operator_s;

typedef struct vkaa_std_type2var_s {
	vkaa_parse_type2var_s type2var;
	vkaa_std_typeid_s *typeid;
} vkaa_std_type2var_s;

typedef vkaa_parse_result_t* (*vkaa_std_keyword_f)(const vkaa_std_keyword_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, vkaa_parse_syntax_t *restrict syntax);
typedef vkaa_parse_result_t* (*vkaa_std_operator_f)(const vkaa_std_operator_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax, const vkaa_parse_result_t *const param_list);
typedef vkaa_parse_result_t* (*vkaa_std_type2var_f)(const vkaa_std_type2var_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax);

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const char *restrict keyword, vkaa_std_keyword_f parse, vkaa_parse_keytype_t keytype);
vkaa_parse_operator_s* vkaa_std_parse_set_operator(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, vkaa_std_operator_f parse, vkaa_parse_optype_t optype, vkaa_parse_optowards_t towards, const char *restrict oplevel, const char *restrict ternary_second_operator);
vkaa_parse_operator_s* vkaa_std_parse_set_operator_unary_left(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards);
vkaa_parse_operator_s* vkaa_std_parse_set_operator_unary_right(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards);
vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards);
vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary_assign(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards);
vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary_selector(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards);
vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary_testeval(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards);
vkaa_parse_s* vkaa_std_parse_set_operator_ternary(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator_first, const char *restrict operator_second, const char *restrict oplevel, vkaa_parse_optowards_t towards);
vkaa_parse_s* vkaa_std_parse_set_operator_ternary_testeval(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator_first, const char *restrict operator_second, const char *restrict oplevel, vkaa_parse_optowards_t towards);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, vkaa_syntax_type_t type, vkaa_std_type2var_f parse, vkaa_parse_type2var_type_t type2var_type);

// other

typedef struct vkaa_std_param_t {
	exbuffer_t buffer;
	vkaa_var_s **input_list;
	uintptr_t input_number;
} vkaa_std_param_t;

vkaa_std_param_t* vkaa_std_param_initial(vkaa_std_param_t *restrict param);
void vkaa_std_param_finally(vkaa_std_param_t *restrict param);
vkaa_std_param_t* vkaa_std_param_push_var(vkaa_std_param_t *restrict param, vkaa_var_s *restrict var);
vkaa_std_param_t* vkaa_std_param_push_syntax(vkaa_std_param_t *restrict param, const vkaa_parse_context_t *restrict context, const vkaa_syntax_s *restrict syntax);

#endif
