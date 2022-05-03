#ifndef _vkaa_std_extern_h_
#define _vkaa_std_extern_h_

#include "vkaa.h"
#include "vkaa.std.h"
#include "vkaa.type.h"
#include "vkaa.var.h"
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

vkaa_type_s* vkaa_std_tpool_set(vkaa_tpool_s *restrict tpool, const char *restrict name, uintptr_t id, vkaa_type_create_f create);

// parse

typedef struct vkaa_std_type2var_with_id_s {
	vkaa_parse_type2var_s type2var;
	uintptr_t id;
} vkaa_std_type2var_with_id_s;

typedef struct vkaa_std_type2var_with_id2_s {
	vkaa_parse_type2var_s type2var;
	uintptr_t id1;
	uintptr_t id2;
} vkaa_std_type2var_with_id2_s;

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword(vkaa_parse_s *restrict p, const char *restrict keyword, vkaa_parse_keyword_f parse, vkaa_parse_keytype_t keytype);
vkaa_parse_operator_s* vkaa_std_parse_set_operator(vkaa_parse_s *restrict p, const vkaa_oplevel_s *restrict opl, const char *restrict operator, vkaa_parse_operator_f parse, vkaa_parse_optype_t optype, const char *restrict oplevel, const char *restrict ternary_second_operator);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var(vkaa_parse_s *restrict p, vkaa_syntax_type_t type, vkaa_parse_type2var_f parse, vkaa_parse_type2var_type_t type2var_type);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_with_id(vkaa_parse_s *restrict p, vkaa_syntax_type_t type, vkaa_parse_type2var_f parse, vkaa_parse_type2var_type_t type2var_type, uintptr_t id);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_with_id2(vkaa_parse_s *restrict p, vkaa_syntax_type_t type, vkaa_parse_type2var_f parse, vkaa_parse_type2var_type_t type2var_type, uintptr_t id1, uintptr_t id2);

#endif
