#include "std.parse.h"

static vkaa_std_keyword_define(var)
{
	const vkaa_syntax_t *restrict s;
	const vkaa_syntax_t *restrict k;
	const vkaa_type_s *restrict type;
	const vkaa_syntax_s *restrict initial;
	vkaa_var_s *restrict var;
	var = NULL;
	if (!(type = vkaa_std_keyword_parse_get_type(context->tpool, context->scope, syntax)))
		goto label_fail;
	while ((k = vkaa_parse_syntax_fetch_and_next(syntax)))
	{
		if (k->type != vkaa_syntax_type_keyword)
			goto label_fail;
		if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)))
			goto label_fail;
		initial = NULL;
		if (s->type == vkaa_syntax_type_brackets)
		{
			initial = s->data.brackets;
			if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)))
				goto label_fail;
		}
		if (!(var = vkaa_tpool_var_create(context->tpool, type, initial)))
			goto label_fail;
		if (!vkaa_scope_put(context->scope, k->data.keyword->string, var))
			goto label_fail;
		refer_free(var);
		var = NULL;
		if (s->type == vkaa_syntax_type_semicolon)
			return result;
		if (s->type != vkaa_syntax_type_comma)
			goto label_fail;
	}
	label_fail:
	if (var) refer_free(var);
	return NULL;
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_var(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "var", vkaa_std_keyword_label(var), vkaa_parse_keytype_complete);
}
