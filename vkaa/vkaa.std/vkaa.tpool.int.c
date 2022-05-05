#include "std.tpool.h"

static vkaa_var_s* vkaa_std_type_int_create(const vkaa_type_s *restrict type)
{
	vkaa_std_var_int_s *restrict r;
	if ((r = (vkaa_std_var_int_s *) refer_alloz(sizeof(vkaa_std_var_int_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(&r->var, type, &r->value))
			return &r->var;
		refer_free(r);
	}
	return NULL;
}

static vkaa_std_function_define(int, op_mov)
{
	vkaa_std_vp(int, 0) = vkaa_std_vp(int, 1);
	return r->output;
}

static vkaa_std_function_define(int, op_add)
{
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) + vkaa_std_vp(int, 1);
	return r->output;
}

static vkaa_std_function_define(int, op_sub)
{
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) - vkaa_std_vp(int, 1);
	return r->output;
}

static vkaa_std_function_define(int, op_mul)
{
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) * vkaa_std_vp(int, 1);
	return r->output;
}

static vkaa_std_function_define(int, op_div)
{
	vkaa_std_int_t v1;
	if ((v1 = vkaa_std_vp(int, 1)))
	{
		vkaa_std_vo(int) = vkaa_std_vp(int, 0) / v1;
		return r->output;
	}
	return NULL;
}

static vkaa_std_function_define(int, cv_bool)
{
	vkaa_std_vo(bool) = !!vkaa_std_vt(int);
	return r->output;
}

static vkaa_std_function_define(int, cv_uint)
{
	vkaa_std_vo(uint) = (vkaa_std_uint_t) vkaa_std_vt(int);
	return r->output;
}

static vkaa_std_function_define(int, cv_int)
{
	vkaa_std_vo(int) = vkaa_std_vt(int);
	return r->output;
}

static vkaa_std_function_define(int, cv_float)
{
	vkaa_std_vo(float) = (vkaa_std_float_t) vkaa_std_vt(int);
	return r->output;
}

static vkaa_type_s* vkaa_std_type_initial_int(vkaa_type_s *restrict type, const vkaa_std_typeid_t *restrict typeid)
{
	uintptr_t tid_2_int_int[] = {typeid->id_int, typeid->id_int};
	if (
		vkaa_std_type_set_function(type, "=", vkaa_std_function_label(int, op_mov), vkaa_std_selector_output_input_first, 0, typeid->id_int, 2, tid_2_int_int) &&
		vkaa_std_type_set_function(type, "+", vkaa_std_function_label(int, op_add), vkaa_std_selector_output_any, 0, typeid->id_int, 2, tid_2_int_int) &&
		vkaa_std_type_set_function(type, "-", vkaa_std_function_label(int, op_sub), vkaa_std_selector_output_any, 0, typeid->id_int, 2, tid_2_int_int) &&
		vkaa_std_type_set_function(type, "*", vkaa_std_function_label(int, op_mul), vkaa_std_selector_output_any, 0, typeid->id_int, 2, tid_2_int_int) &&
		vkaa_std_type_set_function(type, "/", vkaa_std_function_label(int, op_div), vkaa_std_selector_output_any, 0, typeid->id_int, 2, tid_2_int_int) &&
		vkaa_std_type_set_function(type, "bool", vkaa_std_function_label(int, cv_bool), vkaa_std_selector_output_any, typeid->id_int, typeid->id_bool, 0, NULL) &&
		vkaa_std_type_set_function(type, "uint", vkaa_std_function_label(int, cv_uint), vkaa_std_selector_output_any, typeid->id_int, typeid->id_uint, 0, NULL) &&
		vkaa_std_type_set_function(type, "int", vkaa_std_function_label(int, cv_int), vkaa_std_selector_output_any, typeid->id_int, typeid->id_int, 0, NULL) &&
		vkaa_std_type_set_function(type, "float", vkaa_std_function_label(int, cv_float), vkaa_std_selector_output_any, typeid->id_int, typeid->id_float, 0, NULL) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_int(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "int", typeid->id_int, vkaa_std_type_int_create, vkaa_std_type_initial_int, typeid);
}
