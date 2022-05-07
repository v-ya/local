#include "std.parse.h"

vkaa_parse_operator_s* vkaa_std_parse_set_operator(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, vkaa_std_operator_f parse, vkaa_parse_optype_t optype, vkaa_parse_optowards_t towards, const char *restrict oplevel, const char *restrict ternary_second_operator)
{
	const vkaa_level_s *restrict level;
	vkaa_std_operator_s *restrict r;
	vkaa_parse_operator_s *rr;
	rr = NULL;
	if ((level = vkaa_oplevel_get(opl, oplevel)) &&
		(r = (vkaa_std_operator_s *) refer_alloc(sizeof(vkaa_std_operator_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_parse_operator_finally);
		r->operator.parse = (vkaa_parse_operator_f) parse;
		r->operator.op_left_okay_notify = NULL;
		r->operator.optype = optype;
		r->operator.towards = towards;
		r->operator.oplevel = (const vkaa_level_s *) refer_save(level);
		r->operator.ternary_second_operator = ternary_second_operator;
		r->typeid = *typeid;
		if (optype != vkaa_parse_optype_unary_right)
			rr = vkaa_parse_operator_set(p, operator, &r->operator);
		else rr = vkaa_parse_op1unary_set(p, operator, &r->operator);
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

static vkaa_selector_s* vkaa_std_operator_get_selector(vkaa_var_s *restrict p0, vkaa_var_s *restrict this, const char *restrict op)
{
	vkaa_selector_s *restrict s;
	if (!(s = vkaa_var_find_selector(p0, op)))
		s = vkaa_var_find_selector(this, op);
	return s;
}

static vkaa_function_s* vkaa_std_operator_selector_do(const vkaa_selector_s *restrict selector, const vkaa_parse_context_t *restrict context, vkaa_var_s *restrict this, vkaa_var_s **input_list, uintptr_t input_number)
{
	vkaa_selector_param_t param;
	param.tpool = context->tpool;
	param.exec = context->execute;
	param.this = this;
	param.input_list = input_list;
	param.input_number = input_number;
	return selector->selector(selector, &param);
}

static const vkaa_parse_result_t* vkaa_std_operator_get_input_list(const vkaa_parse_result_t *const param_list, vkaa_var_s *input_list[], uintptr_t *restrict input_number)
{
	uintptr_t i, n;
	n = *input_number;
	for (i = 0; param_list[i].type; ++i)
	{
		if (i >= n)
			goto label_fail;
		if (!(input_list[i] = vkaa_parse_result_get_var(&param_list[i])))
			goto label_fail;
	}
	*input_number = i;
	return param_list;
	label_fail:
	*input_number = 0;
	return param_list;
}

static vkaa_std_operator_define(unary_brackets)
{
	vkaa_selector_s *restrict s;
	vkaa_var_s *restrict this;
	vkaa_std_param_t p;
	if ((s = vkaa_std_operator_get_selector_by_result(param_list, &this)) ||
		((this = vkaa_parse_result_get_var(param_list)) &&
			(s = vkaa_std_operator_get_selector(this, context->this, vkaa_parse_operator_brackets))))
	{
		if (vkaa_std_param_initial(&p))
		{
			if (vkaa_std_param_push_syntax(&p, context, syntax->data.brackets))
				result->data.function = vkaa_std_operator_selector_do(s, context, this, p.input_list, p.input_number);
			vkaa_std_param_finally(&p);
			if (result->data.function)
			{
				result->type = vkaa_parse_rtype_function;
				return result;
			}
		}
	}
	return NULL;
}

static vkaa_std_operator_define(unary_square)
{
	vkaa_selector_s *restrict s;
	vkaa_var_s *restrict this;
	const vkaa_syntax_s *restrict square;
	vkaa_var_s *input_list[1];
	vkaa_parse_result_t last_var;
	if ((this = vkaa_parse_result_get_var(param_list)) &&
		(s = vkaa_std_operator_get_selector(this, context->this, vkaa_parse_operator_square)))
	{
		square = syntax->data.square;
		if (vkaa_parse_parse(context, square->syntax_array, square->syntax_number, &last_var))
		{
			input_list[0] = (vkaa_var_s *) refer_save(vkaa_parse_result_get_var(&last_var));
			vkaa_parse_result_clear(&last_var);
			if (input_list[0] && (result->data.function = vkaa_std_operator_selector_do(s, context, this, input_list, 1)))
			{
				result->type = vkaa_parse_rtype_function;
				return result;
			}
		}
	}
	return NULL;
}

static vkaa_std_operator_define(proxy)
{
	vkaa_selector_s *restrict s;
	uintptr_t input_number;
	vkaa_var_s *input_list[3];
	if (syntax->type != vkaa_syntax_type_operator)
		goto label_fail;
	input_number = 3;
	if (!vkaa_std_operator_get_input_list(param_list, input_list, &input_number))
		goto label_fail;
	if (!(s = vkaa_std_operator_get_selector(input_list[0], context->this, syntax->data.operator->string)))
		goto label_fail;
	if ((result->data.function = vkaa_std_operator_selector_do(s, context, NULL, input_list, input_number)))
	{
		result->type = vkaa_parse_rtype_function;
		return result;
	}
	label_fail:
	return NULL;
}

static vkaa_std_operator_define(proxy_preprocess)
{
	vkaa_std_op_pre_t pre;
	const char *restrict name;
	vkaa_selector_s *restrict s;
	uintptr_t input_number;
	vkaa_var_s *input_list[1];
	vkaa_function_s *func;
	if (syntax->type != vkaa_syntax_type_operator)
		goto label_fail;
	if (!(name = vkaa_std_op_pre_name(&pre, syntax->data.operator)))
		goto label_fail;
	input_number = 1;
	if (!vkaa_std_operator_get_input_list(param_list, input_list, &input_number))
		goto label_fail;
	if (!(s = vkaa_std_operator_get_selector(input_list[0], context->this, name)))
		goto label_okay;
	if (!(func = vkaa_std_operator_selector_do(s, context, NULL, input_list, input_number)))
		goto label_fail;
	result->data.var = vkaa_function_okay(func);
	refer_free(func);
	if (result->data.var)
	{
		result->type = vkaa_parse_rtype_var;
		label_okay:
		return result;
	}
	label_fail:
	return NULL;
}

static vkaa_std_operator_define(assign_proxy)
{
	const vkaa_type_s *restrict type;
	vkaa_function_s *restrict func;
	vkaa_var_s *restrict v;
	if ((type = vkaa_parse_result_get_type(param_list)) &&
		param_list->type == vkaa_parse_rtype_function &&
		type == vkaa_parse_result_get_type(param_list + 1) &&
		!(func = param_list[1].data.function)->output &&
		(v = vkaa_parse_result_get_var(param_list)) &&
		vkaa_function_set_output(func, v))
	{
		result->type = vkaa_parse_rtype_var;
		result->data.var = (vkaa_var_s *) refer_save(v);
		return result;
	}
	return vkaa_std_operator_label(proxy)(r, result, context, syntax, param_list);
}

static vkaa_std_operator_define(unary_left_proxy)
{
	switch (syntax->type)
	{
		case vkaa_syntax_type_operator: return vkaa_std_operator_label(proxy)(r, result, context, syntax, param_list);
		case vkaa_syntax_type_brackets: return vkaa_std_operator_label(unary_brackets)(r, result, context, syntax, param_list);
		case vkaa_syntax_type_square: return vkaa_std_operator_label(unary_square)(r, result, context, syntax, param_list);
		default: return NULL;
	}
}

static vkaa_std_operator_define(binary_selector)
{
	vkaa_var_s *restrict this, *restrict keyword;
	refer_nstring_t name;
	vkaa_selector_s *restrict s;
	if ((this = vkaa_parse_result_get_var(param_list)) &&
		(keyword = vkaa_parse_result_get_var(param_list + 1)) &&
		keyword->type_id == r->typeid.id_string &&
		(name = ((vkaa_std_var_string_s *) keyword)->value) &&
		(s = vkaa_var_find_selector(this, name->string)))
	{
		result->type = vkaa_parse_rtype_selector;
		result->data.selector = (vkaa_selector_s *) refer_save(s);
		result->this = (vkaa_var_s *) refer_save(this);
		return result;
	}
	return vkaa_std_operator_label(proxy)(r, result, context, syntax, param_list);
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_unary_left(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	return vkaa_std_parse_set_operator(p, typeid, opl, operator, vkaa_std_operator_label(unary_left_proxy), vkaa_parse_optype_unary_left, towards, oplevel, NULL);
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_unary_right(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	return vkaa_std_parse_set_operator(p, typeid, opl, operator, vkaa_std_operator_label(proxy), vkaa_parse_optype_unary_right, towards, oplevel, NULL);
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	return vkaa_std_parse_set_operator(p, typeid, opl, operator, vkaa_std_operator_label(proxy), vkaa_parse_optype_binary, towards, oplevel, NULL);
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary_assign(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	return vkaa_std_parse_set_operator(p, typeid, opl, operator, vkaa_std_operator_label(assign_proxy), vkaa_parse_optype_binary, towards, oplevel, NULL);
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary_selector(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	return vkaa_std_parse_set_operator(p, typeid, opl, operator, vkaa_std_operator_label(binary_selector), vkaa_parse_optype_binary_second_type2var, towards, oplevel, NULL);
}

vkaa_parse_operator_s* vkaa_std_parse_set_operator_binary_preprocess(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	vkaa_parse_operator_s *restrict r;
	if ((r = vkaa_std_parse_set_operator(p, typeid, opl, operator, vkaa_std_operator_label(proxy), vkaa_parse_optype_binary, towards, oplevel, NULL)))
		r->op_left_okay_notify = (vkaa_parse_operator_f) vkaa_std_operator_label(proxy_preprocess);
	return r;
}

vkaa_parse_s* vkaa_std_parse_set_operator_ternary(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator_first, const char *restrict operator_second, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	if (vkaa_std_parse_set_operator(p, typeid, opl, operator_first, vkaa_std_operator_label(proxy), vkaa_parse_optype_ternary_first, towards, oplevel, operator_second))
	{
		if (vkaa_std_parse_set_operator(p, typeid, opl, operator_second, vkaa_std_operator_label(proxy), vkaa_parse_optype_ternary_second, towards, oplevel, NULL))
			return p;
		vkaa_parse_operator_unset(p, operator_first);
	}
	return NULL;
}

vkaa_parse_s* vkaa_std_parse_set_operator_ternary_preprocess(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, const vkaa_oplevel_s *restrict opl, const char *restrict operator_first, const char *restrict operator_second, const char *restrict oplevel, vkaa_parse_optowards_t towards)
{
	vkaa_parse_operator_s *restrict po;
	if ((po = (vkaa_std_parse_set_operator(p, typeid, opl, operator_first, vkaa_std_operator_label(proxy), vkaa_parse_optype_ternary_first, towards, oplevel, operator_second))))
	{
		po->op_left_okay_notify = (vkaa_parse_operator_f) vkaa_std_operator_label(proxy_preprocess);
		if ((po = (vkaa_std_parse_set_operator(p, typeid, opl, operator_second, vkaa_std_operator_label(proxy), vkaa_parse_optype_ternary_second, towards, oplevel, NULL))))
		{
			po->op_left_okay_notify = (vkaa_parse_operator_f) vkaa_std_operator_label(proxy_preprocess);
			return p;
		}
		vkaa_parse_operator_unset(p, operator_first);
	}
	return NULL;
}
