#include "std.parse.h"
#include "std.function.h"
#include <stdlib.h>

static vkaa_std_keyword_define(label)
{
	const vkaa_syntax_t *restrict s;
	const char *restrict label;
	uintptr_t label_pos;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_keyword)
		goto label_fail;
	label = s->data.keyword->string;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || !vkaa_syntax_test(s, vkaa_syntax_type_operator, ":"))
		goto label_fail;
	label_pos = vkaa_execute_next_pos(context->execute);
	if (vkaa_execute_set_label_without_exist(context->execute, label, label_pos))
		return result;
	label_fail:
	return NULL;
}

static vkaa_std_keyword_define(goto)
{
	const vkaa_syntax_t *restrict s;
	vkaa_function_s *restrict func;
	const char *restrict label;
	uintptr_t jumper_pos;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_keyword)
		goto label_fail;
	label = s->data.keyword->string;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_semicolon)
		goto label_fail;
	jumper_pos = vkaa_execute_next_pos(context->execute);
	if (vkaa_execute_add_jump_any(context->execute, label, jumper_pos))
	{
		if ((func = vkaa_function_alloc(NULL,
			vkaa_std_function_label(void, cj_goto),
			vkaa_tpool_find_id(context->tpool, r->typeid->id_void),
			0, NULL)))
		{
			result->type = vkaa_parse_rtype_function;
			result->data.function = func;
			return result;
		}
	}
	label_fail:
	return NULL;
}

static inline const vkaa_syntax_t* vkaa_std_keyword_goto_stack_label_get_stack_pos(const vkaa_syntax_s *restrict syntax, uintptr_t *restrict stack_pos)
{
	if (syntax->syntax_number == 1)
		return vkaa_syntax_convert_uintptr(syntax->syntax_array, stack_pos, 0);
	return NULL;
}

static vkaa_parse_result_t* vkaa_std_keyword_goto_stack_label(const vkaa_std_keyword_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, vkaa_parse_syntax_t *restrict syntax, const char *restrict label)
{
	const vkaa_syntax_t *restrict s;
	vkaa_function_s *restrict func;
	uintptr_t stack_pos, jumper_pos;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)))
		goto label_fail;
	stack_pos = 0;
	if (s->type == vkaa_syntax_type_brackets)
	{
		if (!vkaa_std_keyword_goto_stack_label_get_stack_pos(s->data.brackets, &stack_pos))
			goto label_fail;
		if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)))
			goto label_fail;
	}
	if (s->type != vkaa_syntax_type_semicolon)
		goto label_fail;
	jumper_pos = vkaa_execute_next_pos(context->execute);
	if (vkaa_execute_jump_to_label(context->execute, label, stack_pos, jumper_pos))
	{
		if ((func = vkaa_function_alloc(NULL,
			vkaa_std_function_label(void, cj_goto),
			vkaa_tpool_find_id(context->tpool, r->typeid->id_void),
			0, NULL)))
		{
			result->type = vkaa_parse_rtype_function;
			result->data.function = func;
			return result;
		}
	}
	label_fail:
	return NULL;
}

static vkaa_std_keyword_define(break)
{
	return vkaa_std_keyword_goto_stack_label(r, result, context, syntax, vkaa_std_label_break);
}

static vkaa_std_keyword_define(continue)
{
	return vkaa_std_keyword_goto_stack_label(r, result, context, syntax, vkaa_std_label_continue);
}

static vkaa_std_keyword_define(return)
{
	vkaa_parse_result_t ret;
	vkaa_var_s *restrict var, *restrict rrr;
	const vkaa_syntax_t *restrict s;
	uintptr_t n;
	vkaa_parse_result_initial(&ret);
	if (!(rrr = vkaa_scope_find_only_this(context->scope, vkaa_std_label_return)))
		goto label_fail;
	if (!(s = vkaa_parse_syntax_get_range(syntax, &n, vkaa_syntax_type_semicolon, NULL)))
		goto label_fail;
	n -= 1;
	if (!vkaa_parse_parse(context, s, n, &ret))
		goto label_fail;
	syntax->pos += n;
	switch (ret.type)
	{
		case vkaa_parse_rtype_none:
			goto label_not_set;
		case vkaa_parse_rtype_var:
		case vkaa_parse_rtype_function:
			if ((var = vkaa_parse_result_get_var(&ret)))
				break;
			// fall through
		default: goto label_fail;
	}
	if (!vkaa_std_convert_set_var(context->execute, context->tpool, context->this, var, rrr))
		goto label_fail;
	label_not_set:
	vkaa_parse_result_clear(&ret);
	return vkaa_std_keyword_goto_stack_label(r, result, context, syntax, vkaa_std_label_return);
	label_fail:
	vkaa_parse_result_clear(&ret);
	return NULL;
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_label(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "label", vkaa_std_keyword_label(label), vkaa_parse_keytype_complete);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_goto(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "goto", vkaa_std_keyword_label(goto), vkaa_parse_keytype_complete);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_break(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "break", vkaa_std_keyword_label(break), vkaa_parse_keytype_complete);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_continue(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "continue", vkaa_std_keyword_label(continue), vkaa_parse_keytype_complete);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_return(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "return", vkaa_std_keyword_label(return), vkaa_parse_keytype_complete);
}
