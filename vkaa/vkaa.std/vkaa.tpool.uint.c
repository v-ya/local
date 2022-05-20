#include "std.tpool.h"
#include "std.function.h"
#include <stdlib.h>

vkaa_std_var_uint_s* vkaa_std_type_uint_create_by_value(const vkaa_type_s *restrict type, vkaa_std_uint_t value)
{
	vkaa_std_var_uint_s *restrict r;
	if ((r = (vkaa_std_var_uint_s *) refer_alloz(sizeof(vkaa_std_var_uint_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(&r->var, type))
		{
			r->value = value;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static vkaa_std_type_create_define(uint)
{
	if (!syntax)
		return &vkaa_std_type_uint_create_by_value(type, 0)->var;
	if (syntax->syntax_number == 1)
	{
		const vkaa_syntax_t *restrict s;
		uintptr_t value;
		s = syntax->syntax_array;
		if (vkaa_syntax_convert_uintptr(s, &value, 0))
			return &vkaa_std_type_uint_create_by_value(type, value)->var;
	}
	return NULL;
}

static vkaa_type_s* vkaa_std_type_initial_uint(vkaa_type_s *restrict type, vkaa_std_typeid_s *restrict typeid)
{
	if (
		vkaa_std_type_set_function(type, "bool", vkaa_std_function_label(uint, cv_bool), vkaa_std_selector_output_any, vkaa_std_selector_convert_none, typeid->id_uint, typeid->id_bool, 0, NULL) &&
		vkaa_std_type_set_function(type, "uint", vkaa_std_function_label(uint, cv_uint), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_uint, typeid->id_uint, 0, NULL) &&
		vkaa_std_type_set_function(type, "int", vkaa_std_function_label(uint, cv_int), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_uint, typeid->id_int, 0, NULL) &&
		vkaa_std_type_set_function(type, "float", vkaa_std_function_label(uint, cv_float), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_uint, typeid->id_float, 0, NULL) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_uint(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "uint", typeid->id_uint, vkaa_std_type_create_label(uint), vkaa_std_type_initial_uint, typeid);
}
