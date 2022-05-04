#include "std.parse.h"

static vkaa_parse_result_t* vkaa_std_keyword_var(const vkaa_parse_keyword_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, vkaa_parse_syntax_t *restrict syntax)
{
	const vkaa_syntax_t *restrict s;
	const vkaa_syntax_t *restrict k;
	const vkaa_type_s *restrict type;
	vkaa_var_s *restrict var;
	var = NULL;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || !vkaa_syntax_test(s, vkaa_syntax_type_operator, "<"))
		goto label_fail;
	if (!(k = vkaa_parse_syntax_fetch_and_next(syntax)) || k->type != vkaa_syntax_type_keyword)
		goto label_fail;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || !vkaa_syntax_test(s, vkaa_syntax_type_operator, ">"))
		goto label_fail;
	if (!(type = vkaa_tpool_find_name(context->tpool, k->data.keyword->string)))
		goto label_fail;
	while ((s = vkaa_parse_syntax_fetch_and_next(syntax)))
	{
		if (s->type != vkaa_syntax_type_keyword)
			goto label_fail;
		if (!(var = vkaa_tpool_var_create(context->tpool, type)))
			goto label_fail;
		if (!vkaa_scope_put(context->scope, s->data.keyword->string, var))
			goto label_fail;
		refer_free(var);
		var = NULL;
		if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)))
			goto label_fail;
		if (s->type == vkaa_syntax_type_semicolon)
			return result;
		if (s->type != vkaa_syntax_type_comma)
			goto label_fail;
	}
	label_fail:
	if (var) refer_free(var);
	return NULL;
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_var(vkaa_parse_s *restrict parse)
{
	return vkaa_std_parse_set_keyword(parse, "var", vkaa_std_keyword_var, vkaa_parse_keytype_complete);
}
