#include "std.tpool.h"

vkaa_std_var_bool_s* vkaa_std_type_bool_create_by_value(const vkaa_type_s *restrict type, vkaa_std_bool_t boolean)
{
	vkaa_std_var_bool_s *restrict r;
	if ((r = (vkaa_std_var_bool_s *) refer_alloz(sizeof(vkaa_std_var_bool_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(&r->var, type))
		{
			r->value = boolean;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

vkaa_std_var_bool_s* vkaa_std_type_bool_create_by_true(const vkaa_type_s *restrict type)
{
	return vkaa_std_type_bool_create_by_value(type, 1);
}

vkaa_std_var_bool_s* vkaa_std_type_bool_create_by_false(const vkaa_type_s *restrict type)
{
	return vkaa_std_type_bool_create_by_value(type, 0);
}

static vkaa_std_type_create_define(bool)
{
	if (!syntax)
		return &vkaa_std_type_bool_create_by_value(type, 0)->var;
	if (syntax->syntax_number == 1)
	{
		const vkaa_syntax_t *restrict s;
		s = syntax->syntax_array;
		if (vkaa_syntax_test(s, vkaa_syntax_type_keyword, "true"))
			return &vkaa_std_type_bool_create_by_true(type)->var;
		else if (vkaa_syntax_test(s, vkaa_syntax_type_keyword, "false"))
			return &vkaa_std_type_bool_create_by_false(type)->var;
	}
	return NULL;
}

static vkaa_std_type_init_define(bool)
{
	if (
		vkaa_std_type_set_function_si(type, "=", sfsi_need2m(bool, op_mov)) &&
		vkaa_std_type_set_function_si(type, "&=", sfsi_need2m(bool, op_and_bitwise)) &&
		vkaa_std_type_set_function_si(type, "|=", sfsi_need2m(bool, op_or_bitwise)) &&
		vkaa_std_type_set_function_si(type, "^=", sfsi_need2m(bool, op_xor_bitwise)) &&
		vkaa_std_type_set_function(type, "string", sf_need0cvn(bool, string, cv_string)) &&
		vkaa_std_type_set_function(type, "bool", sf_need0cvp(bool, bool, cv_bool)) &&
		vkaa_std_type_set_function(type, "uint", sf_need0cvp(bool, uint, cv_uint)) &&
		vkaa_std_type_set_function(type, "int", sf_need0cvp(bool, int, cv_int)) &&
		vkaa_std_type_set_function(type, "float", sf_need0cvp(bool, float, cv_float)) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_bool(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "bool", typeid->id_bool, typeid,
		vkaa_std_type_create_label(bool), vkaa_std_type_init_label(bool));
}
