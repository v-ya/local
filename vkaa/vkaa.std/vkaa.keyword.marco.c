#include "std.parse.h"

static vkaa_std_keyword_define(marco)
{
	vkaa_std_var_marco_s *restrict marco;
	const vkaa_syntax_t *restrict s;
	const vkaa_syntax_t *restrict brackets;
	const vkaa_syntax_s *restrict scope;
	const char *restrict name;
	marco = NULL;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_keyword)
		goto label_fail;
	name = s->data.keyword->string;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_brackets)
		goto label_fail;
	brackets = s;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_scope)
		goto label_fail;
	scope = s->data.scope;
	if (!(marco = (vkaa_std_var_marco_s *) vkaa_tpool_var_create_by_id(context->tpool, r->typeid->id_marco, brackets->data.brackets)))
		goto label_fail;
	marco->scope = (const vkaa_syntax_s *) refer_save(scope);
	if (!vkaa_scope_put(context->scope, name, &marco->var))
		goto label_fail;
	refer_free(marco);
	return result;
	label_fail:
	if (marco) refer_free(marco);
	return NULL;
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_marco(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "marco", vkaa_std_keyword_label(marco), vkaa_parse_keytype_complete);
}
