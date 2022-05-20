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

static vkaa_std_keyword_define(true)
{
	const vkaa_type_s *restrict type;
	if ((type = vkaa_tpool_find_id(context->tpool, r->typeid->id_bool)) &&
		(result->data.var = &vkaa_std_type_bool_create_by_true(type)->var))
	{
		result->type = vkaa_parse_rtype_var;
		return result;
	}
	return NULL;
}

static vkaa_std_keyword_define(false)
{
	const vkaa_type_s *restrict type;
	if ((type = vkaa_tpool_find_id(context->tpool, r->typeid->id_bool)) &&
		(result->data.var = &vkaa_std_type_bool_create_by_false(type)->var))
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

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_true(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "true", vkaa_std_keyword_label(true), vkaa_parse_keytype_inner);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_false(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "false", vkaa_std_keyword_label(false), vkaa_parse_keytype_inner);
}
