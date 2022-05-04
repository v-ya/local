#include "std.parse.h"

vkaa_parse_operator_s* vkaa_std_parse_set_operator(vkaa_parse_s *restrict p, const vkaa_oplevel_s *restrict opl, const char *restrict operator, vkaa_parse_operator_f parse, vkaa_parse_optype_t optype, vkaa_parse_optowards_t towards, const char *restrict oplevel, const char *restrict ternary_second_operator)
{
	const vkaa_level_s *restrict level;
	vkaa_parse_operator_s *restrict r, *rr;
	rr = NULL;
	if ((level = vkaa_oplevel_get(opl, oplevel)) &&
		(r = (vkaa_parse_operator_s *) refer_alloc(sizeof(vkaa_parse_operator_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_parse_operator_finally);
		r->parse = parse;
		r->optype = optype;
		r->towards = towards;
		r->oplevel = (const vkaa_level_s *) refer_save(level);
		r->ternary_second_operator = ternary_second_operator;
		rr = vkaa_parse_operator_set(p, operator, r);
		refer_free(r);
	}
	return rr;
}

static vkaa_selector_s* vkaa_std_operator_get_selector_by_result(const vkaa_parse_result_t *restrict result, vkaa_var_s *restrict *restrict p_this)
{
	if (result->type == vkaa_parse_rtype_selector && result->this)
	{
		*p_this = result->this;
		return result->data.selector;
	}
	return NULL;
}

static vkaa_var_s* vkaa_std_operator_get_var_by_result(const vkaa_parse_result_t *restrict result, const vkaa_tpool_s *restrict tpool, vkaa_scope_s *restrict scope)
{
	switch (result->type)
	{
		case vkaa_parse_rtype_var:
			return result->data.var;
		case vkaa_parse_rtype_function:
			return vkaa_function_okay(result->data.function, tpool, scope);
		default:
			return NULL;
	}
}

static vkaa_function_s* vkaa_std_operator_selector_do(const vkaa_selector_s *restrict selector, const vkaa_parse_context_t *restrict context, vkaa_var_s *restrict this, vkaa_var_s **input_list, uintptr_t input_number)
{
	vkaa_selector_param_t param;
	vkaa_selector_rdata_t rdata;
	vkaa_function_s *restrict r;
	r = NULL;
	param.exec = context->execute;
	param.this = this?this:context->this;
	param.input_list = input_list;
	param.input_number = input_number;
	rdata.function = NULL;
	rdata.output_typeid = 0;
	rdata.output_must = NULL;
	if (selector->selector(selector, &param, &rdata))
		r = vkaa_function_alloc(selector, &param, &rdata);
	if (rdata.output_must)
		refer_free(rdata.output_must);
	return r;
}

static vkaa_var_s** vkaa_std_operator_unary_brackets_parse_param(exbuffer_t *restrict buffer, const vkaa_parse_context_t *restrict context, const vkaa_syntax_s *restrict syntax, uintptr_t *restrict input_number)
{
	vkaa_var_s *v;
	const vkaa_syntax_t *restrict s;
	uintptr_t i, n;
	s = syntax->syntax_array;
	n = syntax->syntax_number;
	while (n)
	{
		for (i = 0; i < n; ++i)
		{
			if (s[i].type == vkaa_syntax_type_comma)
				break;
			else if (s[i].type == vkaa_syntax_type_semicolon)
				goto label_fail;
		}
		vkaa_execute_set_last(context->execute, NULL);
		if (!vkaa_parse_parse(context, s, i))
			goto label_fail;
		if (!(v = context->execute->last_var))
			goto label_fail;
		if (!exbuffer_append(buffer, &v, sizeof(v)))
			goto label_fail;
		if (i < n) ++i;
		s += i;
		n -= i;
	}
	*input_number = buffer->used / sizeof(vkaa_var_s *);
	return (vkaa_var_s **) buffer->data;
	*input_number = 0;
	label_fail:
	return NULL;
}

static vkaa_parse_result_t* vkaa_std_operator_unary_brackets(const vkaa_parse_operator_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax, const vkaa_parse_result_t *restrict param)
{
	vkaa_selector_s *restrict s;
	vkaa_var_s *restrict this;
	vkaa_var_s **input_list;
	uintptr_t input_number;
	exbuffer_t buffer;
	if ((s = vkaa_std_operator_get_selector_by_result(result, &this)) ||
		((this = vkaa_std_operator_get_var_by_result(result, context->tpool, context->scope)) &&
			(s = vkaa_var_find_selector(this, vkaa_parse_operator_brackets))))
	{
		if (exbuffer_init(&buffer, 0))
		{
			if ((input_list = vkaa_std_operator_unary_brackets_parse_param(&buffer, context, syntax->data.brackets, &input_number)))
				result->data.function = vkaa_std_operator_selector_do(s, context, this, input_list, input_number);
			exbuffer_uini(&buffer);
			if (result->data.function)
			{
				result->type = vkaa_parse_rtype_function;
				return result;
			}
		}
	}
	return NULL;
}

static vkaa_parse_result_t* vkaa_std_operator_unary_square(const vkaa_parse_operator_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax, const vkaa_parse_result_t *restrict param)
{
	vkaa_selector_s *restrict s;
	vkaa_var_s *restrict this;
	const vkaa_syntax_s *restrict square;
	vkaa_var_s *input_list[1];
	if ((this = vkaa_std_operator_get_var_by_result(result, context->tpool, context->scope)) &&
		(s = vkaa_var_find_selector(this, vkaa_parse_operator_square)))
	{
		square = syntax->data.square;
		vkaa_execute_set_last(context->execute, NULL);
		if (vkaa_parse_parse(context, square->syntax_array, square->syntax_number) &&
			(input_list[0] = context->execute->last_var))
		{
			if ((result->data.function = vkaa_std_operator_selector_do(s, context, this, input_list, 1)))
			{
				result->type = vkaa_parse_rtype_function;
				return result;
			}
		}
	}
	return NULL;
}

static vkaa_parse_result_t* vkaa_std_operator_proxy(const vkaa_parse_operator_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax, const vkaa_parse_result_t *const param_list)
{
	vkaa_selector_s *restrict s;
	const char *restrict op;
	uintptr_t input_number;
	vkaa_var_s *input_list[3];
	if (syntax->type != vkaa_syntax_type_operator)
		goto label_fail;
	op = syntax->data.operator->string;
	for (input_number = 0; param_list[input_number].type; ++input_number)
	{
		if (input_number >= 3) goto label_fail;
		if (!(input_list[input_number] = vkaa_std_operator_get_var_by_result(&param_list[input_number], context->tpool, context->scope)))
			goto label_fail;
	}
	if (!(s = vkaa_var_find_selector(input_list[0], op)))
		goto label_fail;
	if ((result->data.function = vkaa_std_operator_selector_do(s, context, NULL, input_list, input_number)))
	{
		result->type = vkaa_parse_rtype_function;
		return result;
	}
	label_fail:
	return NULL;
}

static vkaa_parse_result_t* vkaa_std_operator_unary_left_proxy(const vkaa_parse_operator_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax, const vkaa_parse_result_t *const param_list)
{
	switch (syntax->type)
	{
		case vkaa_syntax_type_operator: return vkaa_std_operator_proxy(r, result, context, syntax, param_list);
		case vkaa_syntax_type_brackets: return vkaa_std_operator_unary_brackets(r, result, context, syntax, param_list);
		case vkaa_syntax_type_square: return vkaa_std_operator_unary_square(r, result, context, syntax, param_list);
		default: return NULL;
	}
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_unary_left(vkaa_parse_s *restrict p, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	return vkaa_std_parse_set_operator(p, opl, operator, vkaa_std_operator_unary_left_proxy, vkaa_parse_optype_unary_left, towards, oplevel, NULL);
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_unary_right(vkaa_parse_s *restrict p, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	return vkaa_std_parse_set_operator(p, opl, operator, vkaa_std_operator_proxy, vkaa_parse_optype_unary_right, towards, oplevel, NULL);
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary(vkaa_parse_s *restrict p, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	return vkaa_std_parse_set_operator(p, opl, operator, vkaa_std_operator_proxy, vkaa_parse_optype_binary, towards, oplevel, NULL);
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary_second_type2var(vkaa_parse_s *restrict p, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	return vkaa_std_parse_set_operator(p, opl, operator, vkaa_std_operator_proxy, vkaa_parse_optype_binary_second_type2var, towards, oplevel, NULL);
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary_or_unary_right(vkaa_parse_s *restrict p, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	return vkaa_std_parse_set_operator(p, opl, operator, vkaa_std_operator_proxy, vkaa_parse_optype_binary_or_unary_right, towards, oplevel, NULL);
}

vkaa_parse_s* vkaa_std_parse_set_operator_ternary(vkaa_parse_s *restrict p, const vkaa_oplevel_s *restrict opl, const char *restrict operator_first, const char *restrict operator_second, const char *restrict oplevel, vkaa_parse_optowards_t towards, vkaa_parse_operator_f parse)
{
	if (vkaa_std_parse_set_operator(p, opl, operator_first, parse, vkaa_parse_optype_ternary_first, towards, oplevel, operator_second))
	{
		if (vkaa_std_parse_set_operator(p, opl, operator_second, parse, vkaa_parse_optype_ternary_second, towards, oplevel, NULL))
			return p;
		vkaa_parse_operator_unset(p, operator_first);
	}
	return NULL;
}
