#include "std.tpool.h"

static vkaa_var_s* vkaa_std_type_float_create(const vkaa_type_s *restrict type)
{
	vkaa_std_var_float_s *restrict r;
	if ((r = (vkaa_std_var_float_s *) refer_alloz(sizeof(vkaa_std_var_float_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(&r->var, type, &r->value))
			return &r->var;
		refer_free(r);
	}
	return NULL;
}

static vkaa_std_function_define(float, op_mov)
{
	vkaa_std_vp(float, 0) = vkaa_std_vp(float, 1);
	return r->output;
}

static vkaa_std_function_define(float, op_add)
{
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) + vkaa_std_vp(float, 1);
	return r->output;
}

static vkaa_std_function_define(float, op_sub)
{
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) - vkaa_std_vp(float, 1);
	return r->output;
}

static vkaa_std_function_define(float, op_mul)
{
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) * vkaa_std_vp(float, 1);
	return r->output;
}

static vkaa_std_function_define(float, op_div)
{
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) / vkaa_std_vp(float, 1);
	return r->output;
}

static vkaa_std_function_define(float, cv_bool)
{
	vkaa_std_vo(bool) = !!vkaa_std_vt(float);
	return r->output;
}

static vkaa_std_function_define(float, cv_uint)
{
	vkaa_std_vo(uint) = (vkaa_std_uint_t) vkaa_std_vt(float);
	return r->output;
}

static vkaa_std_function_define(float, cv_int)
{
	vkaa_std_vo(int) = (vkaa_std_int_t) vkaa_std_vt(float);
	return r->output;
}

static vkaa_std_function_define(float, cv_float)
{
	vkaa_std_vo(float) = vkaa_std_vt(float);
	return r->output;
}

static vkaa_type_s* vkaa_std_type_initial_float(vkaa_type_s *restrict type, const vkaa_std_typeid_t *restrict typeid)
{
	uintptr_t tid_2_float_float[] = {typeid->id_float, typeid->id_float};
	if (
		vkaa_std_type_set_function(type, "=", vkaa_std_function_label(float, op_mov), vkaa_std_selector_output_input_first, 0, typeid->id_float, 2, tid_2_float_float) &&
		vkaa_std_type_set_function(type, "+", vkaa_std_function_label(float, op_add), vkaa_std_selector_output_any, 0, typeid->id_float, 2, tid_2_float_float) &&
		vkaa_std_type_set_function(type, "-", vkaa_std_function_label(float, op_sub), vkaa_std_selector_output_any, 0, typeid->id_float, 2, tid_2_float_float) &&
		vkaa_std_type_set_function(type, "*", vkaa_std_function_label(float, op_mul), vkaa_std_selector_output_any, 0, typeid->id_float, 2, tid_2_float_float) &&
		vkaa_std_type_set_function(type, "/", vkaa_std_function_label(float, op_div), vkaa_std_selector_output_any, 0, typeid->id_float, 2, tid_2_float_float) &&
		vkaa_std_type_set_function(type, "bool", vkaa_std_function_label(float, cv_bool), vkaa_std_selector_output_any, typeid->id_float, typeid->id_bool, 0, NULL) &&
		vkaa_std_type_set_function(type, "uint", vkaa_std_function_label(float, cv_uint), vkaa_std_selector_output_any, typeid->id_float, typeid->id_uint, 0, NULL) &&
		vkaa_std_type_set_function(type, "int", vkaa_std_function_label(float, cv_int), vkaa_std_selector_output_any, typeid->id_float, typeid->id_int, 0, NULL) &&
		vkaa_std_type_set_function(type, "float", vkaa_std_function_label(float, cv_float), vkaa_std_selector_output_any, typeid->id_float, typeid->id_float, 0, NULL) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_float(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "float", typeid->id_float, vkaa_std_type_float_create, vkaa_std_type_initial_float, typeid);
}
