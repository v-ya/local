#include "std.parse.h"
#include <stdlib.h>

static vkaa_std_var_function_s* vkaa_std_keyword_func_create_or_find(const vkaa_tpool_s *restrict tpool, vkaa_scope_s *restrict scope, const char *restrict name, uintptr_t id_function)
{
	vkaa_var_s *restrict var;
	if ((var = vkaa_scope_find_only_this(scope, name)))
	{
		if (var->type_id == id_function)
			return (vkaa_std_var_function_s *) refer_save(var);
	}
	else if ((var = vkaa_tpool_var_create_by_id(tpool, id_function, NULL)))
	{
		if (var->type->clear)
		{
			vkaa_vclear_s *restrict vclear;
			if (!(vclear = vkaa_scope_find_vclear(scope)))
				goto label_fail;
			if (!vkaa_vclear_push(vclear, var))
				goto label_fail;
		}
		if (vkaa_scope_put(scope, name, var))
			return (vkaa_std_var_function_s *) var;
		label_fail:
		refer_free(var);
	}
	return NULL;
}

static inline const vkaa_syntax_t* vkaa_std_keyword_func_get_stack_size(const vkaa_syntax_s *restrict syntax_square, uintptr_t *restrict stack_size)
{
	if (syntax_square->syntax_number == 1)
		return vkaa_syntax_convert_uintptr(syntax_square->syntax_array, stack_size, 0);
	return NULL;
}

static vkaa_std_keyword_define(func)
{
	const vkaa_syntax_t *restrict s;
	const vkaa_type_s *restrict type;
	vkaa_std_var_function_s *restrict func;
	uintptr_t stack_size;
	func = NULL;
	stack_size = 1;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || !vkaa_syntax_test(s, vkaa_syntax_type_operator, "<"))
		goto label_fail;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_keyword)
		goto label_fail;
	if (!(type = vkaa_tpool_find_name(context->tpool, s->data.keyword->string)))
		goto label_fail;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || !vkaa_syntax_test(s, vkaa_syntax_type_operator, ">"))
		goto label_fail;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_keyword)
		goto label_fail;
	if (!(func = vkaa_std_keyword_func_create_or_find(context->tpool, context->scope, s->data.keyword->string, r->typeid->id_function)))
		goto label_fail;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_brackets)
		goto label_fail;
	if (!func->input && !func->desc)
	{
		if (!vkaa_std_var_function_set_input(func, context->tpool, s->data.brackets, type->id))
			goto label_fail;
	}
	else
	{
		if (!vkaa_std_var_function_same_input(func, context->tpool, s->data.brackets, type->id))
			goto label_fail;
	}
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)))
		goto label_fail;
	if (s->type == vkaa_syntax_type_semicolon)
		goto label_okay;
	if (s->type == vkaa_syntax_type_square)
	{
		if (!vkaa_std_keyword_func_get_stack_size(s->data.square, &stack_size))
			goto label_fail;
		if (!stack_size)
			goto label_fail;
		if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)))
			goto label_fail;
	}
	if (s->type != vkaa_syntax_type_scope)
		goto label_fail;
	if (!vkaa_std_var_function_set_scope(func, context, s->data.scope, r->typeid->id_scope, stack_size))
		goto label_fail;
	label_okay:
	refer_free(func);
	return result;
	label_fail:
	if (func) refer_free(func);
	return NULL;
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_func(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "func", vkaa_std_keyword_label(func), vkaa_parse_keytype_complete);
}
