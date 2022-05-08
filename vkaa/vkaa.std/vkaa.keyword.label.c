#include "std.parse.h"

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

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_label(vkaa_parse_s *restrict parse, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "label", vkaa_std_keyword_label(label), vkaa_parse_keytype_complete);
}
