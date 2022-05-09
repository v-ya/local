#include "std.tpool.h"
#include "std.function.h"
#include <string.h>

static vkaa_std_type_create_define(bool)
{
	vkaa_std_var_bool_s *restrict r;
	if ((r = (vkaa_std_var_bool_s *) refer_alloz(sizeof(vkaa_std_var_bool_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(&r->var, type))
		{
			if (syntax)
			{
				if (syntax->type != vkaa_syntax_type_keyword)
					goto label_fail;
				if (!strcmp(syntax->data.keyword->string, "true"))
					r->value = 1;
				else if (!strcmp(syntax->data.keyword->string, "false"))
					r->value = 0;
				else goto label_fail;
			}
			return &r->var;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static vkaa_type_s* vkaa_std_type_initial_bool(vkaa_type_s *restrict type, vkaa_std_typeid_s *restrict typeid)
{
	if (
		vkaa_std_type_set_function(type, "bool", vkaa_std_function_label(bool, cv_bool), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_bool, typeid->id_bool, 0, NULL) &&
		vkaa_std_type_set_function(type, "uint", vkaa_std_function_label(bool, cv_uint), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_bool, typeid->id_uint, 0, NULL) &&
		vkaa_std_type_set_function(type, "int", vkaa_std_function_label(bool, cv_int), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_bool, typeid->id_int, 0, NULL) &&
		vkaa_std_type_set_function(type, "float", vkaa_std_function_label(bool, cv_float), vkaa_std_selector_output_any, vkaa_std_selector_convert_promotion, typeid->id_bool, typeid->id_float, 0, NULL) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_bool(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "bool", typeid->id_bool, vkaa_std_type_create_label(bool), vkaa_std_type_initial_bool, typeid);
}
