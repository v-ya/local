#include "std.function.h"
#include <math.h>

vkaa_selector_s* vkaa_std_type_set_selector(vkaa_type_s *restrict type, const char *restrict name, vkaa_selector_f selector)
{
	vkaa_selector_s *s, *sr;
	if (selector && !vkaa_type_find_selector(type, name) &&
		(s = (vkaa_selector_s *) refer_alloz(sizeof(vkaa_selector_s))))
	{
		s->selector = selector;
		sr = vkaa_type_insert_selector(type, name, s);
		refer_free(s);
		return sr;
	}
	return NULL;
}

vkaa_std_selector_s* vkaa_std_type_set_function(vkaa_type_s *restrict type, const char *restrict name, vkaa_function_f function, refer_t pri_data, vkaa_std_selector_output_t output, vkaa_std_selector_convert_t convert, uintptr_t this_typeid, uintptr_t output_typeid, uintptr_t input_number, const uintptr_t *restrict input_typeid)
{
	vkaa_std_selector_s *s, *snew;
	if (!vkaa_type_find_selector(type, name) && (snew = vkaa_std_selector_alloc()))
	{
		s = (vkaa_std_selector_s *) vkaa_type_insert_selector(type, name, &snew->selector);
		refer_free(snew);
		if (s && vkaa_std_selector_append(s, name, function, pri_data, output, convert, this_typeid, output_typeid, input_number, input_typeid))
			return s;
	}
	return NULL;
}

vkaa_std_selector_s* vkaa_std_type_set_function_si(vkaa_type_s *restrict type, const char *restrict name, vkaa_function_f function, refer_t pri_data, vkaa_std_selector_output_t output, vkaa_std_selector_convert_t convert, uintptr_t this_typeid, uintptr_t output_typeid, uintptr_t input_number, uintptr_t input_typeid)
{
	vkaa_std_selector_s *s, *snew;
	if (!vkaa_type_find_selector(type, name) && (snew = vkaa_std_selector_alloc()))
	{
		s = (vkaa_std_selector_s *) vkaa_type_insert_selector(type, name, &snew->selector);
		refer_free(snew);
		if (s && vkaa_std_selector_append_si(s, name, function, pri_data, output, convert, this_typeid, output_typeid, input_number, input_typeid))
			return s;
	}
	return NULL;
}

vkaa_std_selector_s* vkaa_std_type_set_function_any(vkaa_type_s *restrict type, const char *restrict name, vkaa_function_f function, refer_t pri_data, vkaa_std_selector_output_t output, vkaa_std_selector_convert_t convert, uintptr_t this_typeid, uintptr_t output_typeid, const vkaa_std_selector_desc_input_t *restrict must, const vkaa_std_selector_desc_input_t *restrict maybe)
{
	vkaa_std_selector_s *s, *snew;
	if (!vkaa_type_find_selector(type, name) && (snew = vkaa_std_selector_alloc()))
	{
		s = (vkaa_std_selector_s *) vkaa_type_insert_selector(type, name, &snew->selector);
		refer_free(snew);
		if (s && vkaa_std_selector_append_any(s, name, function, pri_data, output, convert, this_typeid, output_typeid, must, maybe))
			return s;
	}
	return NULL;
}

vkaa_function_s* vkaa_std_function_pushed(vkaa_execute_s *restrict exec, vkaa_function_f function, const vkaa_type_s *restrict output_type, vkaa_var_s *restrict this, vkaa_var_s *restrict output)
{
	vkaa_function_s *restrict func, *rr;
	rr = NULL;
	if ((func = vkaa_function_alloc(NULL, function, output_type, 0, NULL)))
	{
		if (this) vkaa_function_set_this(func, this);
		if (!output || vkaa_function_set_output(func, output))
		{
			if (vkaa_execute_push(exec, func))
				rr = func;
		}
		refer_free(func);
	}
	return rr;
}

const vkaa_parse_context_t* vkaa_std_function_pushed_block(const vkaa_parse_context_t *restrict context, vkaa_parse_syntax_t *restrict syntax)
{
	const vkaa_syntax_t *restrict s;
	uintptr_t n;
	if (!(s = vkaa_parse_syntax_get_curr(syntax)))
		goto label_fail;
	if (s->type == vkaa_syntax_type_scope)
	{
		if (!vkaa_parse_parse(context, s, n = 1, NULL))
			goto label_fail;
	}
	else
	{
		if (!(s = vkaa_parse_syntax_get_range(syntax, &n, vkaa_syntax_type_semicolon, NULL)))
			goto label_fail;
		if (!vkaa_parse_parse(context, s, n, NULL))
			goto label_fail;
	}
	syntax->pos += n;
	return context;
	label_fail:
	return NULL;
}

#if vkaa_std_verbose
static void vkaa_std_function_verbose(const vkaa_function_s *restrict function, vkaa_execute_control_t *restrict control, const char *restrict name);
#endif

// !
vkaa_std_function_define(bool, op_not_logic)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = !vkaa_std_vp(bool, 0);
	return 0;
}

// ~
vkaa_std_function_define(uint, op_not_bitwise)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = ~vkaa_std_vp(uint, 0);
	return 0;
}

// **

// *
vkaa_std_function_define(uint, op_mul)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) * vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_mul)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) * vkaa_std_vp(int, 1);
	return 0;
}
vkaa_std_function_define(float, op_mul)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) * vkaa_std_vp(float, 1);
	return 0;
}

// /
vkaa_std_function_define(uint, op_div)
{vkaa_std_verbose_weak
	vkaa_std_uint_t v1;
	if ((v1 = vkaa_std_vp(uint, 1)))
	{
		vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) / v1;
		return 0;
	}
	return vkaa_std_error(div_zero);
}
vkaa_std_function_define(int, op_div)
{vkaa_std_verbose_weak
	vkaa_std_int_t v1;
	if ((v1 = vkaa_std_vp(int, 1)))
	{
		vkaa_std_vo(int) = vkaa_std_vp(int, 0) / v1;
		return 0;
	}
	return vkaa_std_error(div_zero);
}
vkaa_std_function_define(float, op_div)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) / vkaa_std_vp(float, 1);
	return 0;
}

// %
vkaa_std_function_define(uint, op_mod)
{vkaa_std_verbose_weak
	vkaa_std_uint_t v1;
	if ((v1 = vkaa_std_vp(uint, 1)))
	{
		vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) % v1;
		return 0;
	}
	return vkaa_std_error(div_zero);
}
vkaa_std_function_define(int, op_mod)
{vkaa_std_verbose_weak
	vkaa_std_int_t v1;
	if ((v1 = vkaa_std_vp(int, 1)))
	{
		vkaa_std_vo(int) = vkaa_std_vp(int, 0) % v1;
		return 0;
	}
	return vkaa_std_error(div_zero);
}
vkaa_std_function_define(float, op_mod)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = fmod(vkaa_std_vp(float, 0), vkaa_std_vp(float, 1));
	return 0;
}

// +
vkaa_std_function_define(uint, op_add)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) + vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_add)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) + vkaa_std_vp(int, 1);
	return 0;
}
vkaa_std_function_define(float, op_add)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) + vkaa_std_vp(float, 1);
	return 0;
}
vkaa_std_function_define(uint, op_pos)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0);
	return 0;
}
vkaa_std_function_define(int, op_pos)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = vkaa_std_vp(int, 0);
	return 0;
}
vkaa_std_function_define(float, op_pos)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = vkaa_std_vp(float, 0);
	return 0;
}

// -
vkaa_std_function_define(uint, op_sub)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) - vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_sub)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) - vkaa_std_vp(int, 1);
	return 0;
}
vkaa_std_function_define(float, op_sub)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) - vkaa_std_vp(float, 1);
	return 0;
}
vkaa_std_function_define(uint, op_neg)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = -vkaa_std_vp(uint, 0);
	return 0;
}
vkaa_std_function_define(int, op_neg)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = -vkaa_std_vp(int, 0);
	return 0;
}
vkaa_std_function_define(float, op_neg)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = -vkaa_std_vp(float, 0);
	return 0;
}

// &
vkaa_std_function_define(bool, op_and_bitwise)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(bool, 0) & vkaa_std_vp(bool, 1);
	return 0;
}
vkaa_std_function_define(uint, op_and_bitwise)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) & vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_and_bitwise)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) & vkaa_std_vp(int, 1);
	return 0;
}

// |
vkaa_std_function_define(bool, op_or_bitwise)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(bool, 0) | vkaa_std_vp(bool, 1);
	return 0;
}
vkaa_std_function_define(uint, op_or_bitwise)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) | vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_or_bitwise)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) | vkaa_std_vp(int, 1);
	return 0;
}

// ^
vkaa_std_function_define(bool, op_xor_bitwise)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(bool, 0) ^ vkaa_std_vp(bool, 1);
	return 0;
}
vkaa_std_function_define(uint, op_xor_bitwise)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) ^ vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_xor_bitwise)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) ^ vkaa_std_vp(int, 1);
	return 0;
}

// <<
vkaa_std_function_define(uint, op_lshift)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) << (uint32_t) vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_lshift)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) << (int32_t) vkaa_std_vp(int, 1);
	return 0;
}

// >>
vkaa_std_function_define(uint, op_rshift)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) >> (uint32_t) vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_rshift)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) >> (int32_t) vkaa_std_vp(int, 1);
	return 0;
}

// &&
vkaa_std_function_define(bool, op_and_logic_test)
{vkaa_std_verbose_weak
	if (!vkaa_std_vp(bool, 0))
		vkaa_std_jump();
	return 0;
}
vkaa_std_function_define(bool, op_and_logic)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(bool, 0) & vkaa_std_vp(bool, 1);
	return 0;
}

// ||
vkaa_std_function_define(bool, op_or_logic_test)
{vkaa_std_verbose_weak
	if (vkaa_std_vp(bool, 0))
		vkaa_std_jump();
	return 0;
}
vkaa_std_function_define(bool, op_or_logic)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(bool, 0) | vkaa_std_vp(bool, 1);
	return 0;
}

// ^^
vkaa_std_function_define(bool, op_xor_logic)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(bool, 0) ^ vkaa_std_vp(bool, 1);
	return 0;
}

// ==
vkaa_std_function_define(bool, op_rela_equ)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(bool, 0) == vkaa_std_vp(bool, 1);
	return 0;
}
vkaa_std_function_define(uint, op_rela_equ)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(uint, 0) == vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_rela_equ)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(int, 0) == vkaa_std_vp(int, 1);
	return 0;
}
vkaa_std_function_define(float, op_rela_equ)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(float, 0) == vkaa_std_vp(float, 1);
	return 0;
}

// !=
vkaa_std_function_define(bool, op_rela_ne)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(bool, 0) != vkaa_std_vp(bool, 1);
	return 0;
}
vkaa_std_function_define(uint, op_rela_ne)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(uint, 0) != vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_rela_ne)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(int, 0) != vkaa_std_vp(int, 1);
	return 0;
}
vkaa_std_function_define(float, op_rela_ne)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(float, 0) != vkaa_std_vp(float, 1);
	return 0;
}

// >
vkaa_std_function_define(uint, op_rela_gt)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(uint, 0) > vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_rela_gt)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(int, 0) > vkaa_std_vp(int, 1);
	return 0;
}
vkaa_std_function_define(float, op_rela_gt)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(float, 0) > vkaa_std_vp(float, 1);
	return 0;
}

// <
vkaa_std_function_define(uint, op_rela_lt)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(uint, 0) < vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_rela_lt)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(int, 0) < vkaa_std_vp(int, 1);
	return 0;
}
vkaa_std_function_define(float, op_rela_lt)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(float, 0) < vkaa_std_vp(float, 1);
	return 0;
}

// >=
vkaa_std_function_define(uint, op_rela_gte)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(uint, 0) >= vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_rela_gte)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(int, 0) >= vkaa_std_vp(int, 1);
	return 0;
}
vkaa_std_function_define(float, op_rela_gte)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(float, 0) >= vkaa_std_vp(float, 1);
	return 0;
}

// <=
vkaa_std_function_define(uint, op_rela_lte)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(uint, 0) <= vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_rela_lte)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(int, 0) <= vkaa_std_vp(int, 1);
	return 0;
}
vkaa_std_function_define(float, op_rela_lte)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vp(float, 0) <= vkaa_std_vp(float, 1);
	return 0;
}

// =
vkaa_std_function_define(void, op_mov)
{vkaa_std_verbose_weak
	return 0;
}
vkaa_std_function_define(null, op_mov)
{vkaa_std_verbose_weak
	return 0;
}
vkaa_std_function_define(refer, op_mov)
{vkaa_std_verbose_weak
	if (vkaa_std_var_refer_mov(vkaa_std_var(refer, r->input_list[0]), vkaa_std_var(refer, r->input_list[1])))
		return 0;
	return vkaa_std_error(refer_type);
}
vkaa_std_function_define(string, op_mov)
{vkaa_std_verbose_weak
	vkaa_std_var_string_mov(vkaa_std_var(string, r->input_list[0]), vkaa_std_var(string, r->input_list[1]));
	return 0;
}
vkaa_std_function_define(bool, op_mov)
{vkaa_std_verbose_weak
	vkaa_std_vp(bool, 0) = vkaa_std_vp(bool, 1);
	return 0;
}
vkaa_std_function_define(uint, op_mov)
{vkaa_std_verbose_weak
	vkaa_std_vp(uint, 0) = vkaa_std_vp(uint, 1);
	return 0;
}
vkaa_std_function_define(int, op_mov)
{vkaa_std_verbose_weak
	vkaa_std_vp(int, 0) = vkaa_std_vp(int, 1);
	return 0;
}
vkaa_std_function_define(float, op_mov)
{vkaa_std_verbose_weak
	vkaa_std_vp(float, 0) = vkaa_std_vp(float, 1);
	return 0;
}

// conver<bool>
vkaa_std_function_define(bool, cv_bool)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = vkaa_std_vt(bool);
	return 0;
}
vkaa_std_function_define(bool, cv_uint)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = (vkaa_std_uint_t) vkaa_std_vt(bool);
	return 0;
}
vkaa_std_function_define(bool, cv_int)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = (vkaa_std_int_t) vkaa_std_vt(bool);
	return 0;
}
vkaa_std_function_define(bool, cv_float)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = (vkaa_std_float_t) vkaa_std_vt(bool);
	return 0;
}

// conver<uint>
vkaa_std_function_define(uint, cv_bool)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = !!vkaa_std_vt(uint);
	return 0;
}
vkaa_std_function_define(uint, cv_uint)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = vkaa_std_vt(uint);
	return 0;
}
vkaa_std_function_define(uint, cv_int)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = (vkaa_std_int_t) vkaa_std_vt(uint);
	return 0;
}
vkaa_std_function_define(uint, cv_float)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = (vkaa_std_float_t) vkaa_std_vt(uint);
	return 0;
}

// conver<int>
vkaa_std_function_define(int, cv_bool)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = !!vkaa_std_vt(int);
	return 0;
}
vkaa_std_function_define(int, cv_uint)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = (vkaa_std_uint_t) vkaa_std_vt(int);
	return 0;
}
vkaa_std_function_define(int, cv_int)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = vkaa_std_vt(int);
	return 0;
}
vkaa_std_function_define(int, cv_float)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = (vkaa_std_float_t) vkaa_std_vt(int);
	return 0;
}

// conver<float>
vkaa_std_function_define(float, cv_bool)
{vkaa_std_verbose_weak
	vkaa_std_vo(bool) = !!vkaa_std_vt(float);
	return 0;
}
vkaa_std_function_define(float, cv_uint)
{vkaa_std_verbose_weak
	vkaa_std_vo(uint) = (vkaa_std_uint_t) vkaa_std_vt(float);
	return 0;
}
vkaa_std_function_define(float, cv_int)
{vkaa_std_verbose_weak
	vkaa_std_vo(int) = (vkaa_std_int_t) vkaa_std_vt(float);
	return 0;
}
vkaa_std_function_define(float, cv_float)
{vkaa_std_verbose_weak
	vkaa_std_vo(float) = vkaa_std_vt(float);
	return 0;
}

// ()
vkaa_std_function_define(function, do_call)
{vkaa_std_verbose_weak
	vkaa_std_var_function_s *restrict func;
	vkaa_std_var_function_stack_s *restrict stack;
	vkaa_std_var_function_inst_s *restrict inst;
	uintptr_t error;
	func = vkaa_std_var(function, r->this);
	if ((stack = func->stack))
	{
		if (stack->stack_curr < stack->stack_size)
		{
			inst = stack->stack_inst[stack->stack_curr];
			if (vkaa_std_var_function_inst_initial(inst, r))
			{
				stack->stack_curr += 1;
				error = vkaa_execute_do(inst->exec, control->running);
				stack->stack_curr -= 1;
				vkaa_std_var_function_inst_finally(inst);
				return error;
			}
			return vkaa_std_error(function_initial);
		}
		return vkaa_std_error(function_stack);
	}
	return vkaa_std_error(function_empty);
}

// refer
vkaa_std_function_define(refer, op_store)
{vkaa_std_verbose_weak
	vkaa_function_s *restrict mov;
	vkaa_var_s *target;
	mov = (vkaa_function_s *) r->pri_data;
	if ((target = vkaa_std_var(refer, r->input_list[0])->refer_var))
	{
		if ((mov->output == target || vkaa_function_set_output(mov, target)) &&
			(!mov->input_number || mov->input_list[0] == target || vkaa_function_set_input(mov, 0, target)))
			return mov->function(mov, control);
		return vkaa_std_error(refer_type);
	}
	return vkaa_std_error(refer_empty);
}
vkaa_std_function_define(refer, op_load)
{vkaa_std_verbose_weak
	vkaa_function_s *restrict mov;
	vkaa_var_s *target, *output;
	mov = (vkaa_function_s *) r->pri_data;
	if ((target = vkaa_std_var(refer, r->input_list[0])->refer_var))
	{
		output = r->output;
		if ((mov->output == output || vkaa_function_set_output(mov, output)) &&
			(mov->input_list[0] == output || vkaa_function_set_input(mov, 0, output)) &&
			(mov->input_list[1] == target || vkaa_function_set_input(mov, 1, target)))
			return mov->function(mov, control);
		return vkaa_std_error(refer_type);
	}
	return vkaa_std_error(refer_empty);
}

// control
vkaa_std_function_define(void, cj_goto)
{vkaa_std_verbose_weak
	vkaa_std_jump();
	return 0;
}
vkaa_std_function_define(void, cj_if_next)
{vkaa_std_verbose_weak
	if (!vkaa_std_vo(bool))
		vkaa_std_jump();
	return 0;
}
vkaa_std_function_define(void, cj_if_goto)
{vkaa_std_verbose_weak
	if (vkaa_std_vo(bool))
		vkaa_std_njump();
	else vkaa_std_jump();
	return 0;
}
vkaa_std_function_define(void, cj_while_do)
{vkaa_std_verbose_weak
	if (!vkaa_std_vo(bool))
		vkaa_std_jump();
	return 0;
}
vkaa_std_function_define(void, cj_do_while)
{vkaa_std_verbose_weak
	if (vkaa_std_vo(bool))
		vkaa_std_jump();
	return 0;
}

// debug verbose

#if vkaa_std_verbose

#include <stdio.h>
#include <string.h>

static const char* vkaa_std_function_verbose_var(char *restrict buffer, uintptr_t buffer_size, const vkaa_var_s *restrict var)
{
	const vkaa_type_s *restrict type;
	const char *restrict name;
	uintptr_t pos;
	type = var->type;
	name = type->name;
	pos = 0;
	pos += snprintf(buffer + pos, buffer_size - pos, "%8s[%2zu]@%p", name?name:"", type->id, var);
	if (name)
	{
		if (!strcmp(name, "refer"))
		{
			if (vkaa_std_var(refer, var)->refer_type)
			{
				const char *restrict refer_type_name;
				uintptr_t refer_type_id;
				refer_type_id = vkaa_std_var(refer, var)->refer_type->id;
				if (!(refer_type_name = vkaa_std_var(refer, var)->refer_type->name))
					refer_type_name = "";
				pos += snprintf(buffer + pos, buffer_size - pos, " %s[%2zu]", refer_type_name, refer_type_id);
			}
		}
		else if (!strcmp(name, "string"))
		{
			refer_nstring_t v;
			if ((v = vkaa_std_value(string, var)))
				pos += snprintf(buffer + pos, buffer_size - pos, " %s", v->string);
		}
		else if (!strcmp(name, "bool"))
			pos += snprintf(buffer + pos, buffer_size - pos, " %s", vkaa_std_value(bool, var)?"true":"false");
		else if (!strcmp(name, "uint"))
			pos += snprintf(buffer + pos, buffer_size - pos, " %zu", vkaa_std_value(uint, var));
		else if (!strcmp(name, "int"))
			pos += snprintf(buffer + pos, buffer_size - pos, " %zd", vkaa_std_value(int, var));
		else if (!strcmp(name, "float"))
			pos += snprintf(buffer + pos, buffer_size - pos, " %g", vkaa_std_value(float, var));
	}
	return buffer;
}

static void vkaa_std_function_verbose(const vkaa_function_s *restrict function, vkaa_execute_control_t *restrict control, const char *restrict name)
{
	uintptr_t cpos, i;
	intptr_t jpos, njpos;
	char buffer[128];
	cpos = control->next_pos - 1;
	jpos = (intptr_t) control->array[cpos].jump;
	njpos = -1;
	if (control->next_pos < control->number)
		njpos = (intptr_t) control->array[control->next_pos].jump;
	printf("%p[%zu] %s: jump[%zd], next_jump[%zd]\n", control->array, cpos, name, jpos, njpos);
	printf("\toutput: %s\n", vkaa_std_function_verbose_var(buffer, sizeof(buffer), function->output));
	if (function->this)
		printf("\t  this: %s\n", vkaa_std_function_verbose_var(buffer, sizeof(buffer), function->this));
	for (i = 0; i < function->input_number; ++i)
		printf("\t[%4zu]: %s\n", i, vkaa_std_function_verbose_var(buffer, sizeof(buffer), function->input_list[i]));
}

#endif
