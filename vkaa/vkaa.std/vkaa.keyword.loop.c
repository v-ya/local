#include "std.parse.h"
#include "std.function.h"

static vkaa_std_keyword_define(while)
{
	const vkaa_syntax_t *restrict s;
	vkaa_execute_s *restrict exec;
	uintptr_t label_pos_check;
	vkaa_var_s *restrict vbool;
	vkaa_parse_result_t rv;
	exec = context->execute;
	vkaa_parse_result_initial(&rv);
	if (!vkaa_execute_push_label(exec, vkaa_std_label_continue))
		goto label_fail;
	if (!vkaa_execute_push_label(exec, vkaa_std_label_break))
		goto label_fail;
	label_pos_check = vkaa_execute_next_pos(exec);
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_brackets)
		goto label_fail;
	if (!vkaa_parse_parse(context, s->data.brackets->syntax_array, s->data.brackets->syntax_number, &rv))
		goto label_fail;
	if (!(vbool = vkaa_std_convert_result_get_var(&rv, exec, context->tpool, r->typeid->id_bool)))
		goto label_fail;
	if (!vkaa_execute_jump_to_label(exec, vkaa_std_label_break, 0, vkaa_execute_next_pos(exec)))
		goto label_fail;
	if (!vkaa_std_keyword_function_pushed(
		exec, vkaa_std_function_label(void, cj_while_do),
		vbool->type, NULL, vbool))
		goto label_fail;
	vkaa_parse_result_clear(&rv);
	if (!vkaa_std_keyword_function_pushed_block(context, syntax))
		goto label_fail;
	if (!vkaa_execute_jump_to_label(exec, vkaa_std_label_continue, 0, vkaa_execute_next_pos(exec)))
		goto label_fail;
	if (!vkaa_std_keyword_function_pushed(
		exec, vkaa_std_function_label(void, cj_goto),
		vkaa_tpool_find_id(context->tpool, r->typeid->id_void),
		NULL, NULL))
		goto label_fail;
	if (!vkaa_execute_pop_label(exec, vkaa_std_label_continue, label_pos_check))
		goto label_fail;
	if (vkaa_execute_pop_label(exec, vkaa_std_label_break, vkaa_execute_next_pos(exec)))
		return result;
	label_fail:
	vkaa_parse_result_clear(&rv);
	return NULL;
}

static vkaa_std_keyword_define(do)
{
	const vkaa_syntax_t *restrict s;
	vkaa_execute_s *restrict exec;
	uintptr_t label_pos_begin, label_pos_check;
	vkaa_var_s *restrict vbool;
	vkaa_parse_result_t rv;
	exec = context->execute;
	vkaa_parse_result_initial(&rv);
	if (!vkaa_execute_push_label(exec, vkaa_std_label_continue))
		goto label_fail;
	if (!vkaa_execute_push_label(exec, vkaa_std_label_break))
		goto label_fail;
	label_pos_begin = vkaa_execute_next_pos(exec);
	if (!vkaa_std_keyword_function_pushed_block(context, syntax))
		goto label_fail;
	label_pos_check = vkaa_execute_next_pos(exec);
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || !vkaa_syntax_test(s, vkaa_syntax_type_keyword, "while"))
		goto label_fail;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_brackets)
		goto label_fail;
	if (!vkaa_parse_parse(context, s->data.brackets->syntax_array, s->data.brackets->syntax_number, &rv))
		goto label_fail;
	if (!(vbool = vkaa_std_convert_result_get_var(&rv, exec, context->tpool, r->typeid->id_bool)))
		goto label_fail;
	if (!vkaa_std_keyword_function_pushed(
		exec, vkaa_std_function_label(void, cj_do_while),
		vbool->type, NULL, vbool))
		goto label_fail;
	vkaa_execute_get_last_item(exec)->jump = label_pos_begin;
	vkaa_parse_result_clear(&rv);
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_semicolon)
		goto label_fail;
	if (!vkaa_execute_pop_label(exec, vkaa_std_label_continue, label_pos_check))
		goto label_fail;
	if (vkaa_execute_pop_label(exec, vkaa_std_label_break, vkaa_execute_next_pos(exec)))
		return result;
	label_fail:
	vkaa_parse_result_clear(&rv);
	return NULL;
}

typedef struct vkaa_std_keyword_for_param_t {
	const vkaa_syntax_t *restrict syntax_initial;
	uintptr_t number_initial;
	const vkaa_syntax_t *restrict syntax_check;
	uintptr_t number_check;
	const vkaa_syntax_t *restrict syntax_next;
	uintptr_t number_next;
} vkaa_std_keyword_for_param_t;

static vkaa_std_keyword_for_param_t* vkaa_std_keyword_for_param_get(vkaa_std_keyword_for_param_t *restrict p, const vkaa_syntax_t *restrict syntax)
{
	const vkaa_syntax_t *restrict s;
	uintptr_t i, n, i1, i2;
	if (!syntax || syntax->type != vkaa_syntax_type_brackets)
		goto label_fail;
	s = syntax->data.brackets->syntax_array;
	n = syntax->data.brackets->syntax_number;
	i = 0;
	while (i < n && s[i].type != vkaa_syntax_type_semicolon) ++i;
	i1 = i++;
	while (i < n && s[i].type != vkaa_syntax_type_semicolon) ++i;
	i2 = i++;
	while (i < n && s[i].type != vkaa_syntax_type_semicolon) ++i;
	if (i1 < i2 && i2 < i && i == n)
	{
		p->syntax_initial = s;
		p->number_initial = i1;
		p->syntax_check = s + i1 + 1;
		p->number_check = i2 - i1 - 1;
		p->syntax_next = s + i2 + 1;
		p->number_next = n - i2 - 1;
		return p;
	}
	label_fail:
	return NULL;
}

static vkaa_std_keyword_define(for)
{
	vkaa_execute_s *restrict exec;
	uintptr_t label_pos_check, label_pos_continue;
	vkaa_var_s *restrict vbool;
	vkaa_parse_result_t rv;
	vkaa_std_keyword_for_param_t param;
	exec = context->execute;
	vkaa_parse_result_initial(&rv);
	if (!vkaa_execute_push_label(exec, vkaa_std_label_continue))
		goto label_fail;
	if (!vkaa_execute_push_label(exec, vkaa_std_label_break))
		goto label_fail;
	if (!vkaa_std_keyword_for_param_get(&param, vkaa_parse_syntax_fetch_and_next(syntax)))
		goto label_fail;
	// initial
	if (!vkaa_parse_parse(context, param.syntax_initial, param.number_initial, NULL))
		goto label_fail;
	// check
	label_pos_check = vkaa_execute_next_pos(exec);
	if (param.number_check)
	{
		if (!vkaa_parse_parse(context, param.syntax_check, param.number_check, &rv))
			goto label_fail;
		if (!(vbool = vkaa_std_convert_result_get_var(&rv, exec, context->tpool, r->typeid->id_bool)))
			goto label_fail;
	}
	else
	{
		if (!(rv.data.var = vbool = vkaa_tpool_var_create_by_id(context->tpool, r->typeid->id_bool, NULL)))
			goto label_fail;
		rv.type = vkaa_parse_rtype_var;
		vkaa_std_value(bool, vbool) = 1;
	}
	if (!vkaa_execute_jump_to_label(exec, vkaa_std_label_break, 0, vkaa_execute_next_pos(exec)))
		goto label_fail;
	if (!vkaa_std_keyword_function_pushed(
		exec, vkaa_std_function_label(void, cj_while_do),
		vbool->type, NULL, vbool))
		goto label_fail;
	vkaa_parse_result_clear(&rv);
	// do
	if (!vkaa_std_keyword_function_pushed_block(context, syntax))
		goto label_fail;
	// next
	label_pos_continue = vkaa_execute_next_pos(exec);
	if (!vkaa_parse_parse(context, param.syntax_next, param.number_next, NULL))
		goto label_fail;
	if (!vkaa_std_keyword_function_pushed(
		exec, vkaa_std_function_label(void, cj_goto),
		vkaa_tpool_find_id(context->tpool, r->typeid->id_void),
		NULL, NULL))
		goto label_fail;
	vkaa_execute_get_last_item(exec)->jump = label_pos_check;
	if (!vkaa_execute_pop_label(exec, vkaa_std_label_continue, label_pos_continue))
		goto label_fail;
	if (vkaa_execute_pop_label(exec, vkaa_std_label_break, vkaa_execute_next_pos(exec)))
		return result;
	label_fail:
	vkaa_parse_result_clear(&rv);
	return NULL;
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_while(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "while", vkaa_std_keyword_label(while), vkaa_parse_keytype_complete);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_do(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "do", vkaa_std_keyword_label(do), vkaa_parse_keytype_complete);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_for(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "for", vkaa_std_keyword_label(for), vkaa_parse_keytype_complete);
}
