#include "std.parse.h"
#include "std.function.h"

static vkaa_std_keyword_define(if)
{
	const vkaa_syntax_t *restrict s;
	vkaa_execute_s *restrict exec;
	uintptr_t jumper_pos, label_pos;
	vkaa_var_s *restrict vbool;
	vkaa_function_s *restrict func;
	vkaa_parse_result_t rv;
	exec = context->execute;
	vkaa_parse_result_initial(&rv);
	if (!vkaa_execute_push_label(exec, vkaa_std_label_if_end))
		goto label_fail;
	for (;;)
	{
		if (!vkaa_execute_push_label(exec, vkaa_std_label_if_next))
			goto label_fail;
		// get ()
		if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_brackets)
			goto label_fail;
		if (!vkaa_parse_parse(context, s->data.brackets->syntax_array, s->data.brackets->syntax_number, &rv))
			goto label_fail;
		if (!(vbool = vkaa_std_convert_result_get_var(&rv, exec, context->tpool, r->typeid->id_bool)))
			goto label_fail;
		jumper_pos = vkaa_execute_next_pos(exec);
		if (!vkaa_execute_jump_to_label(exec, vkaa_std_label_if_next, 0, jumper_pos))
			goto label_fail;
		if (!(func = vkaa_std_keyword_function_pushed(
			exec, vkaa_std_function_label(void, cj_if_next),
			vbool->type, NULL, vbool)))
			goto label_fail;
		vkaa_parse_result_clear(&rv);
		// get do
		if (!vkaa_std_keyword_function_pushed_block(context, syntax))
			goto label_fail;
		// done goto end
		if (!vkaa_execute_jump_to_label(exec, vkaa_std_label_if_end, 0, vkaa_execute_next_pos(exec)))
			goto label_fail;
		if (!vkaa_std_keyword_function_pushed(
			exec, vkaa_std_function_label(void, cj_goto),
			vkaa_tpool_find_id(context->tpool, r->typeid->id_void),
			NULL, NULL))
			goto label_fail;
		// if goto
		jumper_pos += 1;
		if (exec->execute_array[jumper_pos].func->function == vkaa_std_function_label(void, cj_goto))
			func->function = vkaa_std_function_label(void, cj_if_goto);
		// pop if next
		label_pos = vkaa_execute_next_pos(exec);
		if (!vkaa_execute_pop_label(exec, vkaa_std_label_if_next, label_pos))
			goto label_fail;
		// check else
		if (!(s = vkaa_parse_syntax_get_curr(syntax)))
			break;
		if (!vkaa_syntax_test(s, vkaa_syntax_type_keyword, "else"))
			break;
		if (!(s = vkaa_parse_syntax_next_and_fetch(syntax)))
			goto label_fail;
		if (vkaa_syntax_test(s, vkaa_syntax_type_keyword, "if"))
		{
			// else if
			if (!(s = vkaa_parse_syntax_next_and_fetch(syntax)))
				goto label_fail;
		}
		else
		{
			// else
			if (!vkaa_std_keyword_function_pushed_block(context, syntax))
				goto label_fail;
			break;
		}
	}
	label_pos = vkaa_execute_next_pos(exec);
	if (vkaa_execute_pop_label(exec, vkaa_std_label_if_end, label_pos))
		return result;
	label_fail:
	vkaa_parse_result_clear(&rv);
	return NULL;
}

static vkaa_std_keyword_define(else)
{
	return NULL;
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_if(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "if", vkaa_std_keyword_label(if), vkaa_parse_keytype_complete);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_else(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "else", vkaa_std_keyword_label(else), vkaa_parse_keytype_complete);
}
