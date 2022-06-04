#include "std.tpool.h"

vkaa_std_var_float_s* vkaa_std_type_float_create_by_value(const vkaa_type_s *restrict type, vkaa_std_float_t value)
{
	vkaa_std_var_float_s *restrict r;
	if ((r = (vkaa_std_var_float_s *) refer_alloz(sizeof(vkaa_std_var_float_s))))
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

static vkaa_std_type_create_define(float)
{
	const vkaa_syntax_t *restrict s;
	double value;
	if (!syntax)
		return &vkaa_std_type_float_create_by_value(type, 0)->var;
	s = syntax->syntax_array;
	if (syntax->syntax_number == 1)
	{
		if (vkaa_syntax_convert_double(s, &value))
			return &vkaa_std_type_float_create_by_value(type, value)->var;
	}
	else if (syntax->syntax_number == 2)
	{
		if (vkaa_syntax_test(s, vkaa_syntax_type_operator, "-") &&
			vkaa_syntax_convert_double(s + 1, &value))
			return &vkaa_std_type_float_create_by_value(type, -value)->var;
	}
	return NULL;
}

static vkaa_std_type_init_define(float)
{
	if (
		vkaa_std_type_set_function_si(type, "=", sfsi_need2m(float, op_mov)) &&
		vkaa_std_type_set_function_si(type, "+=", sfsi_need2m(float, op_add)) &&
		vkaa_std_type_set_function_si(type, "-=", sfsi_need2m(float, op_sub)) &&
		vkaa_std_type_set_function_si(type, "*=", sfsi_need2m(float, op_mul)) &&
		vkaa_std_type_set_function_si(type, "/=", sfsi_need2m(float, op_div)) &&
		vkaa_std_type_set_function_si(type, "%=", sfsi_need2m(float, op_mod)) &&
		vkaa_std_type_set_function(type, "string", sf_need0cvn(float, string, cv_string)) &&
		vkaa_std_type_set_function(type, "bool", sf_need0cvn(float, bool, cv_bool)) &&
		vkaa_std_type_set_function(type, "uint", sf_need0cvn(float, uint, cv_uint)) &&
		vkaa_std_type_set_function(type, "int", sf_need0cvn(float, int, cv_int)) &&
		vkaa_std_type_set_function(type, "float", sf_need0cvp(float, float, cv_float)) &&
		vkaa_std_type_set_function(type, "mulpi", sf_need0fna(float, float, fn_mulpi)) &&
		vkaa_std_type_set_function(type, "exp", sf_need0fna(float, float, fn_exp)) &&
		vkaa_std_type_set_function(type, "ln", sf_need0fna(float, float, fn_ln)) &&
		vkaa_std_type_set_function(type, "sin", sf_need0fna(float, float, fn_sin)) &&
		vkaa_std_type_set_function(type, "cos", sf_need0fna(float, float, fn_cos)) &&
		vkaa_std_type_set_function(type, "tan", sf_need0fna(float, float, fn_tan)) &&
		vkaa_std_type_set_function(type, "asin", sf_need0fna(float, float, fn_asin)) &&
		vkaa_std_type_set_function(type, "acos", sf_need0fna(float, float, fn_acos)) &&
		vkaa_std_type_set_function(type, "atan", sf_need0fna(float, float, fn_atan)) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_float(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "float", typeid->id_float, typeid,
		vkaa_std_type_create_label(float), vkaa_std_type_init_label(float));
}
