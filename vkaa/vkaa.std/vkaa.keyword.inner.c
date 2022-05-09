#include "std.parse.h"

static vkaa_std_keyword_define(null)
{
	if ((result->data.var = vkaa_tpool_var_create_by_id(context->tpool, r->typeid->id_null, NULL)))
	{
		result->type = vkaa_parse_rtype_var;
		return result;
	}
	return NULL;
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_null(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "null", vkaa_std_keyword_label(null), vkaa_parse_keytype_inner);
}
