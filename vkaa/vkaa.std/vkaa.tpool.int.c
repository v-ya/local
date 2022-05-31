#include "std.tpool.h"

vkaa_std_var_int_s* vkaa_std_type_int_create_by_value(const vkaa_type_s *restrict type, vkaa_std_int_t value)
{
	vkaa_std_var_int_s *restrict r;
	if ((r = (vkaa_std_var_int_s *) refer_alloz(sizeof(vkaa_std_var_int_s))))
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

static vkaa_std_type_create_define(int)
{
	const vkaa_syntax_t *restrict s;
	intptr_t value;
	if (!syntax)
		return &vkaa_std_type_int_create_by_value(type, 0)->var;
	s = syntax->syntax_array;
	if (syntax->syntax_number == 1)
	{
		if (vkaa_syntax_convert_intptr(s, &value, 0))
			return &vkaa_std_type_int_create_by_value(type, value)->var;
	}
	else if (syntax->syntax_number == 2)
	{
		if (vkaa_syntax_test(s, vkaa_syntax_type_operator, "-") &&
			vkaa_syntax_convert_intptr(s + 1, &value, 0))
			return &vkaa_std_type_int_create_by_value(type, -value)->var;
	}
	return NULL;
}

static vkaa_std_type_init_define(int)
{
	if (
		vkaa_std_type_set_function_si(type, "=", sfsi_need2m(int, op_mov)) &&
		vkaa_std_type_set_function_si(type, "+=", sfsi_need2m(int, op_add)) &&
		vkaa_std_type_set_function_si(type, "-=", sfsi_need2m(int, op_sub)) &&
		vkaa_std_type_set_function_si(type, "*=", sfsi_need2m(int, op_mul)) &&
		vkaa_std_type_set_function_si(type, "/=", sfsi_need2m(int, op_div)) &&
		vkaa_std_type_set_function_si(type, "%=", sfsi_need2m(int, op_mod)) &&
		vkaa_std_type_set_function_si(type, "&=", sfsi_need2m(int, op_and_bitwise)) &&
		vkaa_std_type_set_function_si(type, "|=", sfsi_need2m(int, op_or_bitwise)) &&
		vkaa_std_type_set_function_si(type, "^=", sfsi_need2m(int, op_xor_bitwise)) &&
		vkaa_std_type_set_function_si(type, "<<=", sfsi_need2m(int, op_lshift)) &&
		vkaa_std_type_set_function_si(type, ">>=", sfsi_need2m(int, op_rshift)) &&
		vkaa_std_type_set_function(type, "string", sf_need0cvn(int, string, cv_string)) &&
		vkaa_std_type_set_function(type, "bool", sf_need0cvn(int, bool, cv_bool)) &&
		vkaa_std_type_set_function(type, "uint", sf_need0cvp(int, uint, cv_uint)) &&
		vkaa_std_type_set_function(type, "int", sf_need0cvp(int, int, cv_int)) &&
		vkaa_std_type_set_function(type, "float", sf_need0cvp(int, float, cv_float)) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_int(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "int", typeid->id_int, typeid,
		vkaa_std_type_create_label(int), vkaa_std_type_init_label(int));
}
