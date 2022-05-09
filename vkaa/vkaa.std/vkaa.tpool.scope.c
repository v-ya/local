#include "std.tpool.h"
#include "std.function.h"

static void vkaa_std_var_scope_free_func(vkaa_std_var_scope_s *restrict r)
{
	if (r->scope) refer_free(r->scope);
	vkaa_var_finally(&r->var);
}

vkaa_std_var_scope_s* vkaa_std_type_scope_create_by_parent(const vkaa_type_s *restrict type, vkaa_scope_s *restrict scope)
{
	vkaa_std_var_scope_s *restrict r;
	if ((r = (vkaa_std_var_scope_s *) refer_alloz(sizeof(vkaa_std_var_scope_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_scope_free_func);
		if (vkaa_var_initial(&r->var, type) &&
			(r->scope = vkaa_scope_alloc(scope)))
			return r;
		refer_free(r);
	}
	return NULL;
}

static vkaa_std_type_create_define(scope)
{
	if (!syntax)
		return &vkaa_std_type_scope_create_by_parent(type, NULL)->var;
	return NULL;
}

static vkaa_type_s* vkaa_std_type_initial_scope(vkaa_type_s *restrict type, vkaa_std_typeid_s *restrict typeid)
{
	uintptr_t tid_bool[] = {typeid->id_bool, typeid->id_bool};
	uintptr_t tid_uint[] = {typeid->id_uint, typeid->id_uint};
	uintptr_t tid_int[] = {typeid->id_int, typeid->id_int};
	uintptr_t tid_float[] = {typeid->id_float, typeid->id_float};
	vkaa_std_op_name_t cache;
	vkaa_std_selector_s *restrict s;
	const char *restrict op, *restrict op2;
	if (
		#define need(_t, _n, _o, _l)  vkaa_std_function_label(_t,  _l), _o, vkaa_std_selector_convert_none, 0, typeid->id_##_t, _n, tid_##_t
		#define need1a(_t, _l)        need(_t, 1, vkaa_std_selector_output_any, _l)
		#define need1i(_t, _l)        need(_t, 1, vkaa_std_selector_output_input_first, _l)
		#define need2a(_t, _l)        need(_t, 2, vkaa_std_selector_output_any, _l)
		(s = vkaa_std_type_set_function(type, op = "!",  need1a(bool,  op_not_logic))) &&
		(s = vkaa_std_type_set_function(type, op = "~",  need1a(uint,  op_not_bitwise))) &&
		(s = vkaa_std_type_set_function(type, op = "*",  need2a(uint,  op_mul))) &&
			vkaa_std_selector_append(s,   op,        need2a(int,   op_mul)) &&
			vkaa_std_selector_append(s,   op,        need2a(float, op_mul)) &&
		(s = vkaa_std_type_set_function(type, op = "/",  need2a(uint,  op_div))) &&
			vkaa_std_selector_append(s,   op,        need2a(int,   op_div)) &&
			vkaa_std_selector_append(s,   op,        need2a(float, op_div)) &&
		(s = vkaa_std_type_set_function(type, op = "+",  need2a(uint,  op_add))) &&
			vkaa_std_selector_append(s,   op,        need2a(int,   op_add)) &&
			vkaa_std_selector_append(s,   op,        need2a(float, op_add)) &&
			vkaa_std_selector_append(s,   op,        need1a(uint,  op_pos)) &&
			vkaa_std_selector_append(s,   op,        need1a(int,   op_pos)) &&
			vkaa_std_selector_append(s,   op,        need1a(float, op_pos)) &&
		(s = vkaa_std_type_set_function(type, op = "-",  need2a(uint,  op_sub))) &&
			vkaa_std_selector_append(s,   op,        need2a(int,   op_sub)) &&
			vkaa_std_selector_append(s,   op,        need2a(float, op_sub)) &&
			vkaa_std_selector_append(s,   op,        need1a(uint,  op_neg)) &&
			vkaa_std_selector_append(s,   op,        need1a(int,   op_neg)) &&
			vkaa_std_selector_append(s,   op,        need1a(float, op_neg)) &&
		(op2 = vkaa_std_op_testeval_name(&cache, op = "&&")) &&
			vkaa_std_type_set_function(type, op2,    need1i(bool, op_and_logic_test)) &&
			vkaa_std_type_set_function(type, op,     need2a(bool, op_and_logic)) &&
		(op2 = vkaa_std_op_testeval_name(&cache, op = "||")) &&
			vkaa_std_type_set_function(type, op2,    need1i(bool,  op_or_logic_test)) &&
			vkaa_std_type_set_function(type, op,     need2a(bool,  op_or_logic)) &&
		(s = vkaa_std_type_set_function(type, op = "^^", need2a(bool,  op_xor_logic))) &&
		#undef need1a
		#undef need1i
		#undef need2a
		#undef need
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_scope(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "scope", typeid->id_scope, vkaa_std_type_create_label(scope), vkaa_std_type_initial_scope, typeid);
}
