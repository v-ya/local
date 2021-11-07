#include "cparse.type.h"

cparse_parse_s* cparse_inner_parse_scope_into(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context)
{
	cparse_stack_scope_s *restrict s;
	cparse_scope_s *restrict scope;
	struct cparse_value_s *restrict value;
	cparse_parse_s *ret;
	ret = NULL;
	if ((s = tparse_tstack_push(inst->stack, sizeof(cparse_stack_scope_s), NULL)))
	{
		s->inner_scope_last = context->scope;
		scope = cparse_inner_alloc_scope_empty();
		if (scope)
		{
			scope->code = (refer_nstring_t) refer_save(context->code);
			scope->code_offset = context->pos;
			value = cparse_inner_alloc_value_empty(cparse_value$scope);
			if (value)
			{
				value->data.scope = (cparse_scope_s *) refer_save(scope);
				if (vattr_insert_tail(context->scope, "{}", value))
				{
					s->scope_curr = scope;
					context->scope = scope->scope;
					ret = pri;
				}
				refer_free(value);
			}
			refer_free(scope);
		}
		if (!ret) tparse_tstack_pop(inst->stack);
	}
	return ret;
}

cparse_parse_s* cparse_inner_parse_scope_out(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context)
{
	cparse_stack_scope_s *restrict s;
	if ((s = tparse_tstack_layer(inst->stack, 0, NULL)))
	{
		s->scope_curr->code_length = context->pos - 1 - s->scope_curr->code_offset;
		context->scope = s->inner_scope_last;
		tparse_tstack_pop(inst->stack);
	}
	return pri;
}
