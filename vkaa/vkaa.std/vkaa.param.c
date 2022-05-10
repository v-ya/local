#include "std.h"

vkaa_std_param_t* vkaa_std_param_initial(vkaa_std_param_t *restrict param)
{
	param->input_list = NULL;
	param->input_number = 0;
	if (exbuffer_init(&param->buffer, 0))
		return param;
	return NULL;
}

void vkaa_std_param_finally(vkaa_std_param_t *restrict param)
{
	vkaa_var_s **restrict p;
	uintptr_t i, n;
	if ((p = param->input_list) && (n = param->input_number))
	{
		for (i = 0; i < n; ++i)
			refer_free(p[i]);
	}
	exbuffer_uini(&param->buffer);
}

vkaa_std_param_t* vkaa_std_param_push_var(vkaa_std_param_t *restrict param, vkaa_var_s *restrict var)
{
	vkaa_var_s **restrict p;
	if (var && (param->input_list = p = (vkaa_var_s **) exbuffer_need(&param->buffer, (param->input_number + 1) * sizeof(*p))))
	{
		p[param->input_number++] = (vkaa_var_s *) refer_save(var);
		return param;
	}
	return NULL;
}

vkaa_std_param_t* vkaa_std_param_push_syntax(vkaa_std_param_t *restrict param, const vkaa_parse_context_t *restrict context, const vkaa_syntax_s *restrict syntax)
{
	const vkaa_syntax_t *restrict s;
	uintptr_t i, n;
	vkaa_parse_result_t result;
	s = syntax->syntax_array;
	n = syntax->syntax_number;
	vkaa_parse_result_initial(&result);
	while (n)
	{
		for (i = 0; i < n; ++i)
		{
			if (s[i].type == vkaa_syntax_type_comma)
				break;
			else if (s[i].type == vkaa_syntax_type_semicolon)
				goto label_fail;
		}
		if (!vkaa_parse_parse(context, s, i, &result))
			goto label_fail;
		if (!vkaa_std_param_push_var(param, vkaa_parse_result_get_var(&result)))
			goto label_fail;
		vkaa_parse_result_clear(&result);
		if (i < n) ++i;
		s += i;
		n -= i;
	}
	return param;
	label_fail:
	vkaa_parse_result_clear(&result);
	return NULL;
}
