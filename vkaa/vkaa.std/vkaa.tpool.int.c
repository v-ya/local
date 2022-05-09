#include "std.tpool.h"
#include "std.function.h"
#include <stdlib.h>

static vkaa_std_type_create_define(int)
{
	vkaa_std_var_int_s *restrict r;
	if ((r = (vkaa_std_var_int_s *) refer_alloz(sizeof(vkaa_std_var_int_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(&r->var, type))
		{
			if (syntax)
			{
				char *endptr;
				if (syntax->type != vkaa_syntax_type_number)
					goto label_fail;
				r->value = (vkaa_std_int_t) strtoll(syntax->data.number->string, &endptr, 0);
				if (!endptr || *endptr)
					goto label_fail;
			}
			return &r->var;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static vkaa_type_s* vkaa_std_type_initial_int(vkaa_type_s *restrict type, vkaa_std_typeid_s *restrict typeid)
{
	uintptr_t tid_2_int_int[] = {typeid->id_int, typeid->id_int};
	if (
		vkaa_std_type_set_function(type, "=", vkaa_std_function_label(int, op_mov), vkaa_std_selector_output_input_first, vkaa_std_selector_convert_none, 0, typeid->id_int, 2, tid_2_int_int) &&
		vkaa_std_type_set_function(type, "bool", vkaa_std_function_label(int, cv_bool), vkaa_std_selector_output_any, vkaa_std_selector_convert_none, typeid->id_int, typeid->id_bool, 0, NULL) &&
		vkaa_std_type_set_function(type, "uint", vkaa_std_function_label(int, cv_uint), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_int, typeid->id_uint, 0, NULL) &&
		vkaa_std_type_set_function(type, "int", vkaa_std_function_label(int, cv_int), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_int, typeid->id_int, 0, NULL) &&
		vkaa_std_type_set_function(type, "float", vkaa_std_function_label(int, cv_float), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_int, typeid->id_float, 0, NULL) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_int(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "int", typeid->id_int, vkaa_std_type_create_label(int), vkaa_std_type_initial_int, typeid);
}
