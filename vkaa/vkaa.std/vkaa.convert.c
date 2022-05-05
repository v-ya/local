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

vkaa_var_s* vkaa_std_convert_by_var(vkaa_execute_s *restrict exec, const vkaa_tpool_s *restrict tpool, vkaa_var_s *restrict src, vkaa_var_s *restrict dst)
{
	vkaa_var_s *rr;
	const vkaa_selector_s *restrict s;
	vkaa_function_s *restrict func;
	rr = NULL;
	if (dst->type->name && (s = vkaa_var_find_selector(src, dst->type->name)) &&
		(func = vkaa_std_convert_create_function(exec, tpool, src, s)))
	{
		if (!func->output && vkaa_function_set_output(func, dst))
		{
			if (vkaa_execute_push(exec, func))
				rr = dst;
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
		if (vkaa_execute_push(exec, func))
			rr = func;
		refer_free(func);
	}
	return rr;
}