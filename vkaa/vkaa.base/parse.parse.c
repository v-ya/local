#include "../vkaa.parse.h"
#include "../vkaa.syntax.h"
#include "../vkaa.oplevel.h"
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

static void vkaa_parse_parse_var_reset(vkaa_parse_result_t *restrict var)
{
	if (var->data.none) refer_free(var->data.none);
	if (var->this) refer_free(var->this);
	var->type = vkaa_parse_rtype_none;
	var->data.none = NULL;
	var->this = NULL;
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
	param[i].type = vkaa_parse_rtype_none;
	param[i].data.none = NULL;
	param[i].this = NULL;
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
		case vkaa_parse_optype_binary_or_unary_right:
			var[1] = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_var);
			op1 = vkaa_parse_parse_stack_get(context, layer_number, rpos++, vkaa_parse_stack_type_op);
			if (!op1 || !var[1]) goto label_fail;
			if ((var[0] = vkaa_parse_parse_stack_get(context, layer_number, rpos, vkaa_parse_stack_type_var)))
			{
				++rpos;
				vkaa_parse_parse_stack_repush_fill_param(param, var, 2);
			}
			else vkaa_parse_parse_stack_repush_fill_param(param, var + 1, 1);
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
		rvar.type = vkaa_parse_rtype_none;
		rvar.data.none = NULL;
		if (op->parse(op, &rvar, context, op1->syntax, param))
		{
			while (rpos)
			{
				tparse_tstack_pop(context->stack);
				--rpos;
			}
			if (rvar.type != vkaa_parse_rtype_function || (rvar.data.function &&
				vkaa_execute_push(context->execute, rvar.data.function)))
			{
				context = vkaa_parse_parse_push_var(context, layer_number, &rvar, vkaa_parse_keytype_normal);
				vkaa_parse_parse_var_reset(&rvar);
				return context;
			}
			vkaa_parse_parse_var_reset(&rvar);
		}
	}
	label_fail:
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

static const vkaa_parse_context_t* vkaa_parse_parse_pop_clear(const vkaa_parse_context_t *restrict context, uintptr_t layer_number)
{
	const vkaa_parse_operator_s *restrict op;
	vkaa_parse_stack_t *restrict stack;
	vkaa_function_s *restrict func;
	vkaa_var_s *restrict last_var;
	uintptr_t need_pop;
	while ((op = vkaa_parse_parse_stack_get_last_op(context, layer_number)))
	{
		if (!vkaa_parse_parse_stack_repush(context, layer_number, op->optype))
			goto label_fail;
	}
	switch ((need_pop = tparse_tstack_layer_number(context->stack) - layer_number))
	{
		case 0:
			if (context->execute->execute_number)
			{
				func = context->execute->execute_array[context->execute->execute_number - 1];
				label_set_last_var_by_func:
				if (!(last_var = vkaa_function_okay(func, context->tpool, context->scope)))
					goto label_fail;
				label_set_last_var:
				vkaa_execute_set_last(context->execute, last_var);
			}
			if (need_pop) tparse_tstack_pop(context->stack);
			return context;
		case 1:
			if (!(stack = vkaa_parse_parse_stack_get(context, layer_number, 0, vkaa_parse_stack_type_var)))
				goto label_fail;
			switch (stack->var_type)
			{
				case vkaa_parse_rtype_var:
					last_var = stack->var_data.var;
					goto label_set_last_var;
				case vkaa_parse_rtype_function:
					func = stack->var_data.function;
					goto label_set_last_var_by_func;
				default: break;
			}
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

vkaa_parse_s* vkaa_parse_parse(const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax, uintptr_t number)
{
	const vkaa_syntax_t *restrict s;
	vkaa_parse_keyword_s *restrict k;
	vkaa_parse_operator_s *restrict op;
	vkaa_parse_type2var_s *restrict t2v;
	vkaa_parse_syntax_t ppos;
	vkaa_parse_result_t var;
	uintptr_t layer_number;
	op = NULL;
	ppos.syntax = syntax;
	ppos.number = number;
	ppos.pos = 0;
	var.type = vkaa_parse_rtype_none;
	var.data.none = NULL;
	var.this = NULL;
	layer_number = tparse_tstack_layer_number(context->stack);
	while (ppos.pos < number)
	{
		s = syntax + ppos.pos++;
		if (op && op->optype == vkaa_parse_optype_binary_second_type2var)
			goto label_force_type2var;
		op = NULL;
		switch (s->type)
		{
			case vkaa_syntax_type_keyword:
				if ((k = vkaa_parse_keyword_get(context->parse, s->data.keyword->string)))
				{
					if (k->keytype == vkaa_parse_keytype_complete)
					{
						if (!vkaa_parse_parse_stack_maybe_empty(context, layer_number))
							goto label_fail;
					}
					else if (k->keytype != vkaa_parse_keytype_inner)
						goto label_fail;
					if (!(k->parse(k, &var, context, &ppos)))
						goto label_fail;
					if (k->keytype == vkaa_parse_keytype_inner)
						goto label_keyword_push_var;
				}
				else if ((var.data.var = vkaa_scope_find(context->scope, s->data.keyword->string)))
				{
					var.type = vkaa_parse_rtype_var;
					refer_save(var.data.var);
					label_keyword_push_var:
					if (!vkaa_parse_parse_push_var(context, layer_number, &var, vkaa_parse_keytype_normal))
						goto label_fail;
				}
				else goto label_fail;
				vkaa_parse_parse_var_reset(&var);
				break;
			case vkaa_syntax_type_operator:
				goto label_operator;
			case vkaa_syntax_type_comma:
			case vkaa_syntax_type_semicolon:
				if (!vkaa_parse_parse_pop_clear(context, layer_number))
					goto label_fail;
				break;
			case vkaa_syntax_type_brackets:
			case vkaa_syntax_type_square:
				if (vkaa_parse_parse_stack_last_is_var(context, layer_number))
				{
					label_operator:
					if (!(op = vkaa_parse_parse_get_operator(context, s)))
						goto label_fail;
					if (!vkaa_parse_parse_push_op(context, layer_number, op, s))
						goto label_fail;
					break;
				}
				// fall through
			case vkaa_syntax_type_scope:
			case vkaa_syntax_type_string:
			case vkaa_syntax_type_multichar:
			case vkaa_syntax_type_number:
				label_force_type2var:
				if (!(t2v = vkaa_parse_type2var_get(context->parse, s->type)))
					goto label_fail;
				if (!(t2v->parse(t2v, &var, context, s)))
					goto label_fail;
				if (!vkaa_parse_parse_push_var(context, layer_number, &var, t2v->type))
					goto label_fail;
				vkaa_parse_parse_var_reset(&var);
				break;
			default: goto label_fail;
		}
	}
	if (vkaa_parse_parse_pop_clear(context, layer_number))
		return context->parse;
	label_fail:
	vkaa_parse_parse_var_reset(&var);
	while (tparse_tstack_layer_number(context->stack) > layer_number)
		tparse_tstack_pop(context->stack);
	return NULL;
}
