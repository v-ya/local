#include "std.function.h"

vkaa_std_selector_s* vkaa_std_type_set_function(vkaa_type_s *restrict type, const char *restrict name, vkaa_function_f function, vkaa_std_selector_output_t output, vkaa_std_selector_convert_t convert, uintptr_t this_typeid, uintptr_t output_typeid, uintptr_t input_number, const uintptr_t *restrict input_typeid)
{
	vkaa_std_selector_s *s, *snew;
	if (!vkaa_type_find_selector(type, name) && (snew = vkaa_std_selector_alloc()))
	{
		s = (vkaa_std_selector_s *) vkaa_type_insert_selector(type, name, &snew->selector);
		refer_free(snew);
		if (s && vkaa_std_selector_append(s, name, function, output, convert, this_typeid, output_typeid, input_number, input_typeid))
			return s;
	}
	return NULL;
}

// *
vkaa_std_function_define(uint, op_mul)
{
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) * vkaa_std_vp(uint, 1);
	return r->output;
}
vkaa_std_function_define(int, op_mul)
{
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) * vkaa_std_vp(int, 1);
	return r->output;
}
vkaa_std_function_define(float, op_mul)
{
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) * vkaa_std_vp(float, 1);
	return r->output;
}

// /
vkaa_std_function_define(uint, op_div)
{
	vkaa_std_uint_t v1;
	if ((v1 = vkaa_std_vp(uint, 1)))
	{
		vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) / v1;
		return r->output;
	}
	return NULL;
}
vkaa_std_function_define(int, op_div)
{
	vkaa_std_int_t v1;
	if ((v1 = vkaa_std_vp(int, 1)))
	{
		vkaa_std_vo(int) = vkaa_std_vp(int, 0) / v1;
		return r->output;
	}
	return NULL;
}
vkaa_std_function_define(float, op_div)
{
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) / vkaa_std_vp(float, 1);
	return r->output;
}

// +
vkaa_std_function_define(uint, op_add)
{
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) + vkaa_std_vp(uint, 1);
	return r->output;
}
vkaa_std_function_define(int, op_add)
{
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) + vkaa_std_vp(int, 1);
	return r->output;
}
vkaa_std_function_define(float, op_add)
{
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) + vkaa_std_vp(float, 1);
	return r->output;
}
vkaa_std_function_define(uint, op_pos)
{
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0);
	return r->output;
}
vkaa_std_function_define(int, op_pos)
{
	vkaa_std_vo(int) = vkaa_std_vp(int, 0);
	return r->output;
}
vkaa_std_function_define(float, op_pos)
{
	vkaa_std_vo(float) = vkaa_std_vp(float, 0);
	return r->output;
}

// -
vkaa_std_function_define(uint, op_sub)
{
	vkaa_std_vo(uint) = vkaa_std_vp(uint, 0) - vkaa_std_vp(uint, 1);
	return r->output;
}
vkaa_std_function_define(int, op_sub)
{
	vkaa_std_vo(int) = vkaa_std_vp(int, 0) - vkaa_std_vp(int, 1);
	return r->output;
}
vkaa_std_function_define(float, op_sub)
{
	vkaa_std_vo(float) = vkaa_std_vp(float, 0) - vkaa_std_vp(float, 1);
	return r->output;
}
vkaa_std_function_define(uint, op_neg)
{
	vkaa_std_vo(uint) = -vkaa_std_vp(uint, 0);
	return r->output;
}
vkaa_std_function_define(int, op_neg)
{
	vkaa_std_vo(int) = -vkaa_std_vp(int, 0);
	return r->output;
}
vkaa_std_function_define(float, op_neg)
{
	vkaa_std_vo(float) = -vkaa_std_vp(float, 0);
	return r->output;
}

// =
vkaa_std_function_define(bool, op_mov)
{
	vkaa_std_vp(bool, 0) = vkaa_std_vp(bool, 1);
	return r->output;
}
vkaa_std_function_define(uint, op_mov)
{
	vkaa_std_vp(uint, 0) = vkaa_std_vp(uint, 1);
	return r->output;
}
vkaa_std_function_define(int, op_mov)
{
	vkaa_std_vp(int, 0) = vkaa_std_vp(int, 1);
	return r->output;
}
vkaa_std_function_define(float, op_mov)
{
	vkaa_std_vp(float, 0) = vkaa_std_vp(float, 1);
	return r->output;
}

// conver<bool>
vkaa_std_function_define(bool, cv_bool)
{
	vkaa_std_vo(bool) = vkaa_std_vt(bool);
	return r->output;
}
vkaa_std_function_define(bool, cv_uint)
{
	vkaa_std_vo(uint) = (vkaa_std_uint_t) vkaa_std_vt(bool);
	return r->output;
}
vkaa_std_function_define(bool, cv_int)
{
	vkaa_std_vo(int) = (vkaa_std_int_t) vkaa_std_vt(bool);
	return r->output;
}
vkaa_std_function_define(bool, cv_float)
{
	vkaa_std_vo(float) = (vkaa_std_float_t) vkaa_std_vt(bool);
	return r->output;
}

// conver<uint>
vkaa_std_function_define(uint, cv_bool)
{
	vkaa_std_vo(bool) = !!vkaa_std_vt(uint);
	return r->output;
}
vkaa_std_function_define(uint, cv_uint)
{
	vkaa_std_vo(uint) = vkaa_std_vt(uint);
	return r->output;
}
vkaa_std_function_define(uint, cv_int)
{
	vkaa_std_vo(int) = (vkaa_std_int_t) vkaa_std_vt(uint);
	return r->output;
}
vkaa_std_function_define(uint, cv_float)
{
	vkaa_std_vo(float) = (vkaa_std_float_t) vkaa_std_vt(uint);
	return r->output;
}

// conver<int>
vkaa_std_function_define(int, cv_bool)
{
	vkaa_std_vo(bool) = !!vkaa_std_vt(int);
	return r->output;
}
vkaa_std_function_define(int, cv_uint)
{
	vkaa_std_vo(uint) = (vkaa_std_uint_t) vkaa_std_vt(int);
	return r->output;
}
vkaa_std_function_define(int, cv_int)
{
	vkaa_std_vo(int) = vkaa_std_vt(int);
	return r->output;
}
vkaa_std_function_define(int, cv_float)
{
	vkaa_std_vo(float) = (vkaa_std_float_t) vkaa_std_vt(int);
	return r->output;
}

// conver<float>
vkaa_std_function_define(float, cv_bool)
{
	vkaa_std_vo(bool) = !!vkaa_std_vt(float);
	return r->output;
}
vkaa_std_function_define(float, cv_uint)
{
	vkaa_std_vo(uint) = (vkaa_std_uint_t) vkaa_std_vt(float);
	return r->output;
}
vkaa_std_function_define(float, cv_int)
{
	vkaa_std_vo(int) = (vkaa_std_int_t) vkaa_std_vt(float);
	return r->output;
}
vkaa_std_function_define(float, cv_float)
{
	vkaa_std_vo(float) = vkaa_std_vt(float);
	return r->output;
}
