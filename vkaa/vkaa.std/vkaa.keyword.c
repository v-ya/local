#include "std.parse.h"

static void vkaa_std_keyword_free_func(vkaa_std_keyword_s *restrict r)
{
	if (r->typeid) refer_free(r->typeid);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const char *restrict keyword, vkaa_std_keyword_f parse, vkaa_parse_keytype_t keytype)
{
	vkaa_std_keyword_s *restrict r;
	vkaa_parse_keyword_s *rr;
	rr = NULL;
	if ((r = (vkaa_std_keyword_s *) refer_alloc(sizeof(vkaa_std_keyword_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_keyword_free_func);
		r->keyword.parse = (vkaa_parse_keyword_f) parse;
		r->keyword.keytype = keytype;
		r->typeid = (vkaa_std_typeid_s *) refer_save(typeid);
		rr = vkaa_parse_keyword_set(p, keyword, &r->keyword);
		refer_free(r);
	}
	return rr;
}

const vkaa_type_s* vkaa_std_keyword_parse_get_type(const vkaa_tpool_s *restrict tpool, const vkaa_scope_s *restrict scope, vkaa_parse_syntax_t *restrict syntax)
{
	const vkaa_syntax_t *restrict s;
	const vkaa_syntax_t *restrict k;
	const vkaa_type_s *restrict type;
	vkaa_var_s *restrict var;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)))
		goto label_fail;
	if (!(k = vkaa_parse_syntax_fetch_and_next(syntax)) || k->type != vkaa_syntax_type_keyword)
		goto label_fail;
	if (tpool && vkaa_syntax_test(s, vkaa_syntax_type_operator, "<"))
	{
		if (!(type = vkaa_tpool_find_name(tpool, k->data.keyword->string)))
			goto label_fail;
	}
	else if (scope && vkaa_syntax_test(s, vkaa_syntax_type_operator, "<?"))
	{
		if (!(var = vkaa_scope_find(scope, k->data.keyword->string)))
			goto label_fail;
		type = var->type;
	}
	else goto label_fail;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || !vkaa_syntax_test(s, vkaa_syntax_type_operator, ">"))
		goto label_fail;
	return type;
	label_fail:
	return NULL;
}

vkaa_function_s* vkaa_std_keyword_function_pushed(vkaa_execute_s *restrict exec, vkaa_function_f function, const vkaa_type_s *restrict output_type, vkaa_var_s *restrict this, vkaa_var_s *restrict output)
{
	vkaa_function_s *restrict func, *rr;
	rr = NULL;
	if ((func = vkaa_function_alloc(NULL, function, output_type, 0, NULL)))
	{
		if (this) vkaa_function_set_this(func, this);
		if (!output || vkaa_function_set_output(func, output))
		{
			if (vkaa_execute_push(exec, func))
				rr = func;
		}
		refer_free(func);
	}
	return rr;
}

const vkaa_parse_context_t* vkaa_std_keyword_function_pushed_block(const vkaa_parse_context_t *restrict context, vkaa_parse_syntax_t *restrict syntax)
{
	const vkaa_syntax_t *restrict s;
	uintptr_t n;
	if (!(s = vkaa_parse_syntax_get_curr(syntax)))
		goto label_fail;
	if (s->type == vkaa_syntax_type_scope)
	{
		if (!vkaa_parse_parse(context, s, n = 1, NULL))
			goto label_fail;
	}
	else
	{
		if (!(s = vkaa_parse_syntax_get_range(syntax, &n, vkaa_syntax_type_semicolon, NULL)))
			goto label_fail;
		if (!vkaa_parse_parse(context, s, n, NULL))
			goto label_fail;
	}
	syntax->pos += n;
	return context;
	label_fail:
	return NULL;
}
