#include "std.tpool.h"

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

static vkaa_std_type_init_define(uint)
{
	if (
		vkaa_std_type_set_function_si(type, "=", sfsi_need2m(uint, op_mov)) &&
		vkaa_std_type_set_function_si(type, "+=", sfsi_need2m(uint, op_add)) &&
		vkaa_std_type_set_function_si(type, "-=", sfsi_need2m(uint, op_sub)) &&
		vkaa_std_type_set_function_si(type, "*=", sfsi_need2m(uint, op_mul)) &&
		vkaa_std_type_set_function_si(type, "/=", sfsi_need2m(uint, op_div)) &&
		vkaa_std_type_set_function_si(type, "%=", sfsi_need2m(uint, op_mod)) &&
		vkaa_std_type_set_function_si(type, "&=", sfsi_need2m(uint, op_and_bitwise)) &&
		vkaa_std_type_set_function_si(type, "|=", sfsi_need2m(uint, op_or_bitwise)) &&
		vkaa_std_type_set_function_si(type, "^=", sfsi_need2m(uint, op_xor_bitwise)) &&
		vkaa_std_type_set_function_si(type, "<<=", sfsi_need2m(uint, op_lshift)) &&
		vkaa_std_type_set_function_si(type, ">>=", sfsi_need2m(uint, op_rshift)) &&
		vkaa_std_type_set_function(type, "bool", sf_need0cvp(uint, bool, cv_bool)) &&
		vkaa_std_type_set_function(type, "uint", sf_need0cvp(uint, uint, cv_uint)) &&
		vkaa_std_type_set_function(type, "int", sf_need0cvp(uint, int, cv_int)) &&
		vkaa_std_type_set_function(type, "float", sf_need0cvp(uint, float, cv_float)) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_uint(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "uint", typeid->id_uint, typeid,
		vkaa_std_type_create_label(uint), vkaa_std_type_init_label(uint));
}
