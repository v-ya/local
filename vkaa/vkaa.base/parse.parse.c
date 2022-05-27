#include "../vkaa.parse.h"
#include "../vkaa.syntax.h"
#include "../vkaa.oplevel.h"
#include "../vkaa.error.h"
#include "../vkaa.tpool.h"
#include "../vkaa.scope.h"
#include "../vkaa.selector.h"
#include "../vkaa.function.h"
#include "../vkaa.execute.h"

typedef enum vkaa_parse_stack_type_t {
	vkaa_parse_stack_type_var               = 0x00,
	vkaa_parse_stack_type_op                = 0x01,
	vkaa_parse_stack_type_mask              = 0x0f,
	vkaa_parse_stack_type_flag_allow_ignore = 0x10,
} vkaa_parse_stack_type_t;

typedef struct vkaa_parse_stack_t {
	vkaa_parse_stack_type_t type;
	vkaa_parse_rtype_t var_type;
	vkaa_parse_rdata_t var_data;
	const vkaa_parse_operator_s *op;
	const vkaa_syntax_t *syntax;
	vkaa_var_s *this;
} vkaa_parse_stack_t;

static void vkaa_parse_stack_free_func(vkaa_parse_stack_t *restrict stack)
{
	if (stack->var_data.none) refer_free(stack->var_data.none);
	if (stack->op) refer_free(stack->op);
	if (stack->this) refer_free(stack->this);
}

static const vkaa_parse_context_t* vkaa_parse_parse_stack_allow_push_var(const vkaa_parse_context_t *restrict context, uintptr_t layer_number)
{
	vkaa_parse_stack_t *restrict stack;
	vkaa_parse_stack_type_t type;
	if (tparse_tstack_layer_number(context->stack) <= layer_number)
		goto label_okay;
	if ((stack = (vkaa_parse_stack_t *) tparse_tstack_layer(context->stack, 0, NULL)))
	{
		type = stack->type & vkaa_parse_stack_type_mask;
		if (type == vkaa_parse_stack_type_op)
		{
			label_okay:
			return context;
		}
		else if (type == vkaa_parse_stack_type_var && (stack->type & vkaa_parse_stack_type_flag_allow_ignore))
		{
			tparse_tstack_pop(context->stack);
			goto label_okay;
		}
	}
	return NULL;
}

static const vkaa_parse_context_t* vkaa_parse_parse_stack_last_is_var(const vkaa_parse_context_t *restrict context, uintptr_t layer_number)
{
	vkaa_parse_stack_t *restrict stack;
	vkaa_parse_stack_type_t type;
	if (tparse_tstack_layer_number(context->stack) > layer_number &&
		(stack = (vkaa_parse_stack_t *) tparse_tstack_layer(context->stack, 0, NULL)))
	{
		type = stack->type & vkaa_parse_stack_type_mask;
		if (type == vkaa_parse_stack_type_var ||
			(type == vkaa_parse_stack_type_op && stack->op->optype == vkaa_parse_optype_unary_left))
			return context;
	}
	return NULL;
}

static const vkaa_parse_context_t* vkaa_parse_parse_stack_maybe_empty(const vkaa_parse_context_t *restrict context, uintptr_t layer_number)
{
	vkaa_parse_stack_t *restrict stack;
	uintptr_t curr_layer_number;
	curr_layer_number = tparse_tstack_layer_number(context->stack);
	if (curr_layer_number == layer_number || (curr_layer_number == layer_number + 1 &&
		(stack = (vkaa_parse_stack_t *) tparse_tstack_layer(context->stack, 0, NULL)) &&
		(stack->type & vkaa_parse_stack_type_flag_allow_ignore)))
		return context;
	return NULL;
}

static vkaa_parse_stack_t* vkaa_parse_parse_stack_get(const vkaa_parse_context_t *restrict context, uintptr_t layer_number, uintptr_t rpos, vkaa_parse_stack_type_t type)
{
	vkaa_parse_stack_t *restrict stack;
	if (tparse_tstack_layer_number(context->stack) > layer_number + rpos &&
		(stack = tparse_tstack_layer(context->stack, rpos, NULL)) &&
		(stack->type & vkaa_parse_stack_type_mask) == type)
		return stack;
	return NULL;
}

static const vkaa_parse_operator_s* vkaa_parse_parse_stack_get_last_op(const vkaa_parse_context_t *restrict context, uintptr_t layer_number)
{
	vkaa_parse_stack_t *restrict stack;
	uintptr_t rpos;
	rpos = 0;
	while (tparse_tstack_layer_number(context->stack) > layer_number + rpos)
	{
		if (!(stack = tparse_tstack_layer(context->stack, rpos++, NULL)))
			break;
		if (stack->type == vkaa_parse_stack_type_op)
			return stack->op;
	}
	return NULL;
}

static const vkaa_parse_context_t* vkaa_parse_parse_check_op_ternary(const vkaa_parse_context_t *restrict context, const vkaa_parse_operator_s *op_first, const vkaa_parse_operator_s *op_second)
{
	const char *restrict op_second_name;
	if (op_first->optype == vkaa_parse_optype_ternary_first &&
		op_second->optype == vkaa_parse_optype_ternary_second &&
		(op_second_name = op_first->ternary_second_operator) &&
		vkaa_parse_operator_get(context->parse, op_second_name) == op_second)
		return context;
	return NULL;
}

static const vkaa_parse_context_t* vkaa_parse_parse_push_var(const vkaa_parse_context_t *restrict context, uintptr_t layer_number, const vkaa_parse_result_t *restrict rvar, vkaa_parse_type2var_type_t type2var_type)
{
	vkaa_parse_stack_t *restrict stack;
	vkaa_parse_stack_type_t type;
	switch (rvar->type)
	{
		case vkaa_parse_rtype_var:
		case vkaa_parse_rtype_selector:
		case vkaa_parse_rtype_function:
			if (rvar->data.none && vkaa_parse_parse_stack_allow_push_var(context, layer_number))
			{
				type = vkaa_parse_stack_type_var;
				if (type2var_type == vkaa_parse_keytype_first_allow_ignore &&
					tparse_tstack_layer_number(context->stack) == layer_number)
					type |= vkaa_parse_stack_type_flag_allow_ignore;
				if ((stack = (vkaa_parse_stack_t *) tparse_tstack_push(context->stack, sizeof(vkaa_parse_stack_t), (tparse_tstack_free_f) vkaa_parse_stack_free_func)))
				{
					stack->type = type;
					stack->var_type = rvar->type;
					stack->var_data.none = refer_save(rvar->data.none);
					stack->this = (vkaa_var_s *) refer_save(rvar->this);
					return context;
				}
			}
			// fall through
		default: return NULL;
	}
}

static void vkaa_parse_parse_stack_repush_fill_param(vkaa_parse_result_t *restrict param, vkaa_parse_stack_t *restrict var[], uintptr_t nparam)
{
	uintptr_t i;
	for (i = 0; i < nparam; ++i)
	{
		param[i].type = var[i]->var_type;
		param[i].data = var[i]->var_data;
		param[i].this = var[i]->this;
	}
	vkaa_parse_result_initial(param + i);
}

static const vkaa_parse_context_t* vkaa_parse_parse_stack_repush(const vkaa_parse_context_t *restrict context, uintptr_t layer_number, vkaa_parse_optype_t optype)
{
	const vkaa_parse_operator_s *restrict op;
	vkaa_parse_stack_t *restrict op1;
	vkaa_parse_stack_t *restrict op2;
	vkaa_parse_stack_t *restrict var[3];
	vkaa_parse_result_t param[4];
	vkaa_parse_result_t rvar;
	uintptr_t rpos;
	rpos = 0;
	switch (optype)
	{
		case vkaa_parse_optype_unary_left:
			op1 = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_op);
			var[0] = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_var);
			if (!op1 || !var[0]) goto label_fail;
			vkaa_parse_parse_stack_repush_fill_param(param, var, 1);
			break;
		case vkaa_parse_optype_unary_right:
			var[0] = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_var);
			op1 = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_op);
			if (!op1 || !var[0]) goto label_fail;
			vkaa_parse_parse_stack_repush_fill_param(param, var, 1);
			break;
		case vkaa_parse_optype_binary:
		case vkaa_parse_optype_binary_second_type2var:
			var[1] = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_var);
			op1 = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_op);
			var[0] = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_var);
			if (!op1 || !var[0] || !var[1]) goto label_fail;
			vkaa_parse_parse_stack_repush_fill_param(param, var, 2);
			break;
		case vkaa_parse_optype_ternary_second:
			optype = vkaa_parse_optype_ternary_first;
			var[2] = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_var);
			op2 = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_op);
			var[1] = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_var);
			op1 = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_op);
			var[0] = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_var);
			if (!op1 || !op2 || !var[0] || !var[1] || !var[2]) goto label_fail;
			if (!vkaa_parse_parse_check_op_ternary(context, op1->op, op2->op)) goto label_fail;
			vkaa_parse_parse_stack_repush_fill_param(param, var, 3);
			break;
		default: goto label_fail;
	}
	if ((op = op1->op)->optype == optype && op->parse)
	{
		vkaa_parse_result_initial(&rvar);
		if (op->parse(op, &rvar, context, op1->syntax, param))
		{
			while (rpos)
			{
				tparse_tstack_pop(context->stack);
				--rpos;
			}
			if (rvar.type != vkaa_parse_rtype_function || (rvar.data.function && vkaa_execute_push(context->execute, rvar.data.function)))
			{
				context = vkaa_parse_parse_push_var(context, layer_number, &rvar, vkaa_parse_keytype_normal);
				vkaa_parse_result_clear(&rvar);
				return context;
			}
		}
		vkaa_parse_result_clear(&rvar);
	}
	label_fail:
	return NULL;
}

static const vkaa_parse_context_t* vkaa_parse_parse_push_op_notify_left_okay(const vkaa_parse_context_t *restrict context, uintptr_t layer_number, const vkaa_parse_operator_s *restrict op, const vkaa_syntax_t *restrict syntax)
{
	vkaa_parse_stack_t *restrict var;
	vkaa_parse_result_t param[2];
	vkaa_parse_result_t rvar;
	vkaa_parse_result_initial(&rvar);
	switch (op->optype)
	{
		case vkaa_parse_optype_unary_right:
			var = NULL;
			vkaa_parse_parse_stack_repush_fill_param(param, &var, 0);
			break;
		case vkaa_parse_optype_unary_left:
		case vkaa_parse_optype_binary:
		case vkaa_parse_optype_binary_second_type2var:
		case vkaa_parse_optype_ternary_first:
		case vkaa_parse_optype_ternary_second:
			var = vkaa_parse_parse_stack_get(context, layer_number, 0, vkaa_parse_stack_type_var);
			if (!var) goto label_fail;
			vkaa_parse_parse_stack_repush_fill_param(param, &var, 1);
			break;
		default: goto label_fail;
	}
	if (op->op_left_okay_notify)
	{
		if (op->op_left_okay_notify(op, &rvar, context, syntax, param))
		{
			if (rvar.type)
			{
				if (rvar.type == vkaa_parse_rtype_function)
				{
					if (!rvar.data.function)
						goto label_fail;
					if (!vkaa_execute_push(context->execute, rvar.data.function))
						goto label_fail;
				}
				if (var->var_data.none)
				{
					refer_free(var->var_data.none);
					var->var_data.none = NULL;
				}
				if (var->this)
				{
					refer_free(var->this);
					var->this = NULL;
				}
				var->var_type = rvar.type;
				var->var_data.none = refer_save(rvar.data.none);
				var->this = (vkaa_var_s *) refer_save(rvar.this);
			}
			vkaa_parse_result_clear(&rvar);
			return context;
		}
	}
	label_fail:
	vkaa_parse_result_clear(&rvar);
	return NULL;
}

static const vkaa_parse_context_t* vkaa_parse_parse_push_op(const vkaa_parse_context_t *restrict context, uintptr_t layer_number, const vkaa_parse_operator_s *restrict op, const vkaa_syntax_t *restrict syntax)
{
	vkaa_parse_stack_t *restrict stack;
	const vkaa_parse_operator_s *restrict op_last;
	if (!op->oplevel->level) goto label_fail;
	for (;;)
	{
		if (!(op_last = vkaa_parse_parse_stack_get_last_op(context, layer_number)) ||
			op_last->oplevel->level < op->oplevel->level ||
			(op_last->oplevel->level == op->oplevel->level &&
				op->towards == vkaa_parse_optowards_right2left))
		{
			label_push_op:
			if (op->op_left_okay_notify && !vkaa_parse_parse_push_op_notify_left_okay(context, layer_number, op, syntax))
				goto label_fail;
			if (!(stack = (vkaa_parse_stack_t *) tparse_tstack_push(context->stack, sizeof(vkaa_parse_stack_t), (tparse_tstack_free_f) vkaa_parse_stack_free_func)))
				goto label_fail;
			stack->type = vkaa_parse_stack_type_op;
			stack->op = (const vkaa_parse_operator_s *) refer_save(op);
			stack->syntax = syntax;
			return context;
		}
		if (op_last->optype != vkaa_parse_optype_ternary_first)
		{
			if (!vkaa_parse_parse_stack_repush(context, layer_number, op_last->optype))
				goto label_fail;
		}
		else
		{
			if (vkaa_parse_parse_check_op_ternary(context, op_last, op))
				goto label_push_op;
			goto label_fail;
		}
	}
	label_fail:
	return NULL;
}

static const vkaa_parse_context_t* vkaa_parse_parse_pop_clear(const vkaa_parse_context_t *restrict context, uintptr_t layer_number, vkaa_parse_result_t *restrict result)
{
	const vkaa_parse_operator_s *restrict op;
	vkaa_parse_stack_t *restrict stack;
	while ((op = vkaa_parse_parse_stack_get_last_op(context, layer_number)))
	{
		if (!vkaa_parse_parse_stack_repush(context, layer_number, op->optype))
			goto label_fail;
	}
	switch (tparse_tstack_layer_number(context->stack) - layer_number)
	{
		case 1:
			if (!(stack = vkaa_parse_parse_stack_get(context, layer_number, 0, vkaa_parse_stack_type_var)))
				goto label_fail;
			if (result)
			{
				result->type = stack->var_type;
				result->data.none = refer_save(stack->var_data.none);
				result->this = (vkaa_var_s *) refer_save(stack->this);
			}
			tparse_tstack_pop(context->stack);
			// fall through
		case 0: return context;
		default: break;
	}
	label_fail:
	return NULL;
}

static vkaa_parse_operator_s* vkaa_parse_parse_get_operator(const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax)
{
	const char *restrict op;
	switch (syntax->type)
	{
		case vkaa_syntax_type_operator: op = syntax->data.operator->string; break;
		case vkaa_syntax_type_brackets: op = vkaa_parse_operator_brackets; break;
		case vkaa_syntax_type_square: op = vkaa_parse_operator_square; break;
		default: goto label_fail;
	}
	return vkaa_parse_operator_get(context->parse, op);
	label_fail:
	return NULL;
}

static vkaa_parse_operator_s* vkaa_parse_parse_get_op1unary(const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax)
{
	if (syntax->type == vkaa_syntax_type_operator)
		return vkaa_parse_op1unary_get(context->parse, syntax->data.operator->string);
	return NULL;
}

const vkaa_parse_context_t* vkaa_parse_parse(const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax, uintptr_t number, vkaa_parse_result_t *restrict result)
{
	const vkaa_syntax_t *restrict s;
	vkaa_parse_keyword_s *restrict k;
	vkaa_parse_operator_s *restrict op;
	vkaa_parse_type2var_s *restrict t2v;
	vkaa_parse_syntax_t ppos;
	vkaa_parse_result_t var;
	uintptr_t layer_number;
	const char *restrict ctime_message;
	op = NULL;
	ctime_message = NULL;
	ppos.syntax = syntax;
	ppos.number = number;
	ppos.pos = 0;
	vkaa_parse_result_initial(&var);
	if (result) vkaa_parse_result_initial(result);
	layer_number = tparse_tstack_layer_number(context->stack);
	if (vkaa_execute_elog_push(context->execute, context->source, syntax, number))
	{
		while (ppos.pos < number)
		{
			s = syntax + ppos.pos++;
			if (op)
			{
				if (op->optype == vkaa_parse_optype_binary_second_type2var)
				{
					op = NULL;
					goto label_force_type2var;
				}
				op = NULL;
			}
			switch (s->type)
			{
				case vkaa_syntax_type_keyword:
					if ((k = vkaa_parse_keyword_get(context->parse, s->data.keyword->string)))
					{
						label_parse_keyword:
						if (k->keytype == vkaa_parse_keytype_complete)
						{
							if (!vkaa_parse_parse_stack_maybe_empty(context, layer_number))
								goto label_fail_keyword_curr_not_complete;
						}
						else if (k->keytype != vkaa_parse_keytype_inner)
							goto label_fail_unknow;
						if (!(k->parse(k, &var, context, &ppos)))
							goto label_fail_keyword_parse;
						if (k->keytype == vkaa_parse_keytype_inner)
							goto label_keyword_push_var;
						if (var.type == vkaa_parse_rtype_function)
						{
							if (!var.data.function)
								goto label_fail_unknow;
							if (!vkaa_execute_push(context->execute, var.data.function))
								goto label_fail_maybe_memoryless;
						}
						if (!vkaa_execute_elog_fence(context->execute, context->source, syntax, number, ppos.pos))
							goto label_fail_maybe_memoryless;
					}
					else if ((var.data.var = vkaa_scope_find(context->scope, s->data.keyword->string)))
					{
						var.type = vkaa_parse_rtype_var;
						refer_save(var.data.var);
						label_keyword_push_var:
						if (!vkaa_parse_parse_push_var(context, layer_number, &var, vkaa_parse_keytype_normal))
							goto label_fail_push_var;
					}
					else goto label_fail_keyword_not_find;
					vkaa_parse_result_clear(&var);
					break;
				case vkaa_syntax_type_operator:
					if (vkaa_parse_parse_stack_last_is_var(context, layer_number))
						goto label_operator;
					else if ((op = vkaa_parse_parse_get_op1unary(context, s)))
						goto label_op1unary;
					else
					{
						label_try_operator_as_keyword:
						if ((k = vkaa_parse_keyword_get(context->parse, s->data.operator->string)))
							goto label_parse_keyword;
						goto label_fail_operator;
					}
				case vkaa_syntax_type_comma:
				case vkaa_syntax_type_semicolon:
					if (!vkaa_parse_parse_pop_clear(context, layer_number, NULL))
						goto label_fail_pop_clear;
					if (!vkaa_execute_elog_fence(context->execute, context->source, syntax, number, ppos.pos))
						goto label_fail_maybe_memoryless;
					break;
				case vkaa_syntax_type_brackets:
				case vkaa_syntax_type_square:
					if (vkaa_parse_parse_stack_last_is_var(context, layer_number))
					{
						label_operator:
						if (!(op = vkaa_parse_parse_get_operator(context, s)))
							goto label_try_operator_as_keyword;
						label_op1unary:
						if (!vkaa_parse_parse_push_op(context, layer_number, op, s))
							goto label_fail_push_operator;
						break;
					}
					// fall through
				case vkaa_syntax_type_scope:
				case vkaa_syntax_type_string:
				case vkaa_syntax_type_multichar:
				case vkaa_syntax_type_number:
					label_force_type2var:
					if (!(t2v = vkaa_parse_type2var_get(context->parse, s->type)))
						goto label_fail_type2var;
					if (!(t2v->parse(t2v, &var, context, s)))
						goto label_fail_type2var_parse;
					if (!vkaa_parse_parse_push_var(context, layer_number, &var, t2v->type))
						goto label_fail_push_var;
					vkaa_parse_result_clear(&var);
					break;
				default: goto label_fail_unknow;
			}
		}
		if (!vkaa_parse_parse_pop_clear(context, layer_number, result))
			goto label_fail_pop_clear;
		vkaa_execute_elog_pop(context->execute, syntax, number);
		return context;
		label_fail_do:
		vkaa_execute_elog_pop(context->execute, syntax, number);
	}
	vkaa_parse_result_clear(&var);
	while (tparse_tstack_layer_number(context->stack) > layer_number)
		tparse_tstack_pop(context->stack);
	return NULL;
	label_fail_unknow:
	label_fail:
	if (number)
	{
		if (ppos.pos) ppos.pos -= 1;
		if (ppos.pos < number)
			s = &syntax[ppos.pos];
		else s = &syntax[number - 1];
		if (!ctime_message)
			ctime_message = "c-error: unknow error";
		vkaa_execute_elog_print_ctime(context->execute, context->source, s->pos, ctime_message);
	}
	goto label_fail_do;
	label_fail_maybe_memoryless:
	ctime_message = "c-error: maybe memory less";
	goto label_fail;
	label_fail_keyword_curr_not_complete:
	ctime_message = "c-error: complete keyword maybe not at here";
	goto label_fail;
	label_fail_keyword_parse:
	ctime_message = "c-error: keyword parse fail";
	goto label_fail;
	label_fail_keyword_not_find:
	ctime_message = "c-error: keyword or var-name not find";
	goto label_fail;
	label_fail_operator:
	ctime_message = "c-error: operator parse fail";
	goto label_fail;
	label_fail_type2var:
	ctime_message = "c-error: type2var parse not find";
	goto label_fail;
	label_fail_type2var_parse:
	ctime_message = "c-error: type2var parse fail";
	goto label_fail;
	label_fail_push_var:
	ctime_message = "c-error: push var to parse stack fail";
	goto label_fail;
	label_fail_push_operator:
	ctime_message = "c-error: push operator to parse stack fail";
	goto label_fail;
	label_fail_pop_clear:
	ctime_message = "c-error: pop clear parse stack fail";
	goto label_fail;
}
