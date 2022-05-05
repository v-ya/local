#include "std.parse.h"

static vkaa_parse_result_t* vkaa_std_keyword_null(const vkaa_std_keyword_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, vkaa_parse_syntax_t *restrict syntax)
{
	const vkaa_syntax_t *restrict name;
	if ((name = vkaa_parse_syntax_get_last(syntax)) && name->type == vkaa_syntax_type_keyword)
	{
		if ((result->data.var = vkaa_tpool_var_create_by_name(context->tpool, name->data.keyword->string)))
		{
			result->type = vkaa_parse_rtype_var;
			return result;
		}
	}
	return NULL;
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_null(vkaa_parse_s *restrict parse, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "null", vkaa_std_keyword_null, vkaa_parse_keytype_inner);
}
