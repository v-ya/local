#include "std.h"
#include <string.h>

const vkaa_selector_s* vkaa_std_convert_test_by_type(const vkaa_type_s *restrict src, const vkaa_type_s *restrict dst)
{
	if (dst->name) return vkaa_type_find_selector(src, dst->name);
	return NULL;
}

const vkaa_selector_s* vkaa_std_convert_test_by_name(const vkaa_type_s *restrict src, const char *restrict dst)
{
	if (dst) return vkaa_type_find_selector(src, dst);
	return NULL;
}

const vkaa_selector_s* vkaa_std_convert_test_by_typeid(const vkaa_type_s *restrict src, uintptr_t dst, const vkaa_tpool_s *restrict tpool)
{
	const vkaa_type_s *restrict dt;
	if ((dt = vkaa_tpool_find_id(tpool, dst)) && dt->name)
		return vkaa_type_find_selector(src, dt->name);
	return NULL;
}

static vkaa_function_s* vkaa_std_convert_create_function(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_var_s *restrict src, const vkaa_selector_s *restrict s)
{
	vkaa_selector_param_t param;
	param.tpool = tpool;
	param.exec = exec;
	param.this = src;
	param.input_list = NULL;
	param.input_number = 0;
	return s->selector(s, &param);
}

vkaa_function_s* vkaa_std_convert_by_var(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_var_s *src, vkaa_var_s *dst)
{
	vkaa_function_s *rr;
	const vkaa_selector_s *restrict s;
	vkaa_function_s *restrict func;
	const char *restrict name;
	rr = NULL;
	if ((name = dst->type->name) && (s = vkaa_var_find_selector(src, name)) &&
		(func = vkaa_std_convert_create_function(exec, tpool, src, s)))
	{
		if (!func->output && vkaa_function_set_output(func, dst))
		{
			if (vkaa_execute_push(exec, func))
				rr = func;
		}
		refer_free(func);
	}
	return rr;
}

vkaa_function_s* vkaa_std_convert_by_typeid(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_var_s *restrict src, uintptr_t dst)
{
	vkaa_function_s *rr;
	const vkaa_type_s *restrict dt;
	const vkaa_selector_s *restrict s;
	vkaa_function_s *restrict func;
	rr = NULL;
	if ((dt = vkaa_tpool_find_id(tpool, dst)) && dt->name && (s = vkaa_var_find_selector(src, dt->name)) &&
		(func = vkaa_std_convert_create_function(exec, tpool, src, s)))
	{
		if (func->output_type == dt && vkaa_execute_push(exec, func))
			rr = func;
		refer_free(func);
	}
	return rr;
}

vkaa_function_s* vkaa_std_convert_mov_var(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_var_s *restrict this, vkaa_var_s *src, vkaa_var_s *dst)
{
	static const char op_name[] = "=";
	vkaa_function_s *rr;
	vkaa_function_s *restrict func;
	vkaa_selector_s *restrict s;
	vkaa_selector_param_t param;
	vkaa_var_s *input_list[2];
	rr = NULL;
	if ((s = vkaa_var_find_selector(dst, op_name)) ||
		(s = vkaa_var_find_selector(this, op_name)))
	{
		input_list[0] = dst;
		input_list[1] = src;
		param.tpool = tpool;
		param.exec = exec;
		param.this = NULL;
		param.input_list = input_list;
		param.input_number = 2;
		if ((func = s->selector(s, &param)))
		{
			if (func->output == dst && vkaa_execute_push(exec, func))
				rr = func;
			refer_free(func);
		}
	}
	return rr;
}

vkaa_function_s* vkaa_std_convert_set_var(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_var_s *restrict this, vkaa_var_s *src, vkaa_var_s *dst)
{
	if (src->type_id == dst->type_id)
		return vkaa_std_convert_mov_var(exec, tpool, this, src, dst);
	return vkaa_std_convert_by_var(exec, tpool, src, dst);
}

vkaa_var_s* vkaa_std_convert_result_get_var(const vkaa_parse_result_t *restrict result, vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, uintptr_t type_id)
{
	vkaa_var_s *restrict r;
	vkaa_function_s *restrict func;
	if ((r = vkaa_parse_result_get_var(result)))
	{
		if (r->type_id == type_id)
			goto label_okay;
		if ((func = vkaa_std_convert_by_typeid(exec, tpool, r, type_id)) &&
			(r = vkaa_function_okay(func)))
		{
			label_okay:
			return r;
		}
	}
	return NULL;
}
