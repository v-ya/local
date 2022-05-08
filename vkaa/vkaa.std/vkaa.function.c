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
		if (!vkaa_parse_parse(context, s->data.scope->syntax_array, s->data.scope->syntax_number, NULL))
			goto label_fail;
		syntax->pos += 1;
	}
	else
	{
		if (!(s = vkaa_parse_syntax_get_range(syntax, &n, vkaa_syntax_type_semicolon, NULL)))
			goto label_fail;
		if (!vkaa_parse_parse(context, s, n, NULL))
			goto label_fail;
		syntax->pos += n;
	}
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

// =
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

// debug verbose

#if vkaa_std_verbose

#include <stdio.h>
#include <string.h>

static const char* vkaa_std_function_verbose_var(char *restrict buffer, const vkaa_var_s *restrict var)
{
	const vkaa_type_s *restrict type;
	const char *restrict name;
	char *restrict p;
	type = var->type;
	name = type->name;
	p = buffer;
	p += sprintf(p, "%8s[%2zu]@%p", name?name:"", type->id, var);
	if (name)
	{
		if (!strcmp(name, "string"))
		{
			refer_nstring_t v;
			if ((v = vkaa_std_value(string, var)))
				p += sprintf(" %s", v->string);
		}
		else if (!strcmp(name, "bool"))
			p += sprintf(p, " %s", vkaa_std_value(bool, var)?"true":"false");
		else if (!strcmp(name, "uint"))
			p += sprintf(p, " %zu", vkaa_std_value(uint, var));
		else if (!strcmp(name, "int"))
			p += sprintf(p, " %zd", vkaa_std_value(int, var));
		else if (!strcmp(name, "float"))
			p += sprintf(p, " %g", vkaa_std_value(float, var));
	}
	return buffer;
}

static void vkaa_std_function_verbose(const vkaa_function_s *restrict function, vkaa_execute_control_t *restrict control, const char *restrict name)
{
	uintptr_t cpos, i;
	intptr_t jpos, njpos;
	char buffer[256];
	cpos = control->next_pos - 1;
	jpos = (intptr_t) control->array[cpos].jump;
	njpos = -1;
	if (control->next_pos < control->number)
		njpos = (intptr_t) control->array[control->next_pos].jump;
	printf("[%zu] %s: jump[%zd], next_jump[%zd]\n", cpos, name, jpos, njpos);
	printf("\toutput: %s\n", vkaa_std_function_verbose_var(buffer, function->output));
	if (function->this)
		printf("\t  this: %s\n", vkaa_std_function_verbose_var(buffer, function->this));
	for (i = 0; i < function->input_number; ++i)
		printf("\t[%4zu]: %s\n", i, vkaa_std_function_verbose_var(buffer, function->input_list[i]));
}

#endif
