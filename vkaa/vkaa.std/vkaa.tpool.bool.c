#include "std.tpool.h"
#include "std.function.h"

static vkaa_var_s* vkaa_std_type_bool_create(const vkaa_type_s *restrict type)
{
	vkaa_std_var_bool_s *restrict r;
	if ((r = (vkaa_std_var_bool_s *) refer_alloz(sizeof(vkaa_std_var_bool_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(&r->var, type))
			return &r->var;
		refer_free(r);
	}
	return NULL;
}

static vkaa_type_s* vkaa_std_type_initial_bool(vkaa_type_s *restrict type, const vkaa_std_typeid_t *restrict typeid)
{
	uintptr_t tid_2_bool_bool[] = {typeid->id_bool, typeid->id_bool};
	if (
		vkaa_std_type_set_function(type, "=", vkaa_std_function_label(bool, op_mov), vkaa_std_selector_output_input_first, vkaa_std_selector_convert_none, 0, typeid->id_bool, 2, tid_2_bool_bool) &&
		vkaa_std_type_set_function(type, "bool", vkaa_std_function_label(bool, cv_bool), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_bool, typeid->id_bool, 0, NULL) &&
		vkaa_std_type_set_function(type, "uint", vkaa_std_function_label(bool, cv_uint), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_bool, typeid->id_uint, 0, NULL) &&
		vkaa_std_type_set_function(type, "int", vkaa_std_function_label(bool, cv_int), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_bool, typeid->id_int, 0, NULL) &&
		vkaa_std_type_set_function(type, "float", vkaa_std_function_label(bool, cv_float), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_bool, typeid->id_float, 0, NULL) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_bool(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "bool", typeid->id_bool, vkaa_std_type_bool_create, vkaa_std_type_initial_bool, typeid);
}
