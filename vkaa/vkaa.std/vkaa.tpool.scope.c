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
		if (vkaa_var_initial(&r->var, type, NULL) &&
			(r->scope = vkaa_scope_alloc(scope)))
			return r;
		refer_free(r);
	}
	return NULL;
}

static vkaa_var_s* vkaa_std_type_scope_create(const vkaa_type_s *restrict type)
{
	return &vkaa_std_type_scope_create_by_parent(type, NULL)->var;
}

static vkaa_type_s* vkaa_std_type_initial_scope(vkaa_type_s *restrict type, const vkaa_std_typeid_t *restrict typeid)
{
	// uintptr_t tid_2_bool_bool[] = {typeid->id_bool, typeid->id_bool};
	uintptr_t tid_2_uint_uint[] = {typeid->id_uint, typeid->id_uint};
	uintptr_t tid_2_int_int[] = {typeid->id_int, typeid->id_int};
	uintptr_t tid_2_float_float[] = {typeid->id_float, typeid->id_float};
	vkaa_std_selector_output_t o = vkaa_std_selector_output_any;
	vkaa_std_selector_s *restrict s;
	const char *restrict op;
	if (
		#define need1(_t, _n)  vkaa_std_function_label(_t,  _n), o, 0, typeid->id_##_t, 1, tid_2_##_t##_##_t
		#define need2(_t, _n)  vkaa_std_function_label(_t,  _n), o, 0, typeid->id_##_t, 2, tid_2_##_t##_##_t
		(s = vkaa_std_type_set_function(type, op = "*", need2(uint,  op_mul))) &&
			vkaa_std_selector_append(s,   op,       need2(int,   op_add)) &&
			vkaa_std_selector_append(s,   op,       need2(float, op_add)) &&
		(s = vkaa_std_type_set_function(type, op = "/", need2(uint,  op_div))) &&
			vkaa_std_selector_append(s,   op,       need2(int,   op_add)) &&
			vkaa_std_selector_append(s,   op,       need2(float, op_add)) &&
		(s = vkaa_std_type_set_function(type, op = "+", need2(uint,  op_add))) &&
			vkaa_std_selector_append(s,   op,       need2(int,   op_add)) &&
			vkaa_std_selector_append(s,   op,       need2(float, op_add)) &&
			vkaa_std_selector_append(s,   op,       need1(uint,  op_pos)) &&
			vkaa_std_selector_append(s,   op,       need1(int,   op_pos)) &&
			vkaa_std_selector_append(s,   op,       need1(float, op_pos)) &&
		(s = vkaa_std_type_set_function(type, op = "-", need2(uint,  op_sub))) &&
			vkaa_std_selector_append(s,   op,       need2(int,   op_add)) &&
			vkaa_std_selector_append(s,   op,       need2(float, op_add)) &&
			vkaa_std_selector_append(s,   op,       need1(uint,  op_neg)) &&
			vkaa_std_selector_append(s,   op,       need1(int,   op_neg)) &&
			vkaa_std_selector_append(s,   op,       need1(float, op_neg)) &&
		#undef need1
		#undef need2
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_scope(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "scope", typeid->id_scope, vkaa_std_type_scope_create, vkaa_std_type_initial_scope, typeid);
}
