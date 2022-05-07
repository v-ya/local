#include "std.parse.h"
#include <string.h>

static vkaa_std_type2var_define(string)
{
	vkaa_var_s *restrict v;
	switch (syntax->type)
	{
		case vkaa_syntax_type_keyword:
		case vkaa_syntax_type_string:
			if ((v = vkaa_tpool_var_create_by_id(context->tpool, r->typeid.id_string, syntax)))
			{
				result->type = vkaa_parse_rtype_var;
				result->data.var = v;
				return result;
			}
			// fall through
		default: return NULL;
	}
}

static vkaa_std_type2var_define(number)
{
	const char *restrict s;
	vkaa_var_s *restrict v;
	if (syntax->type == vkaa_syntax_type_number)
	{
		s = syntax->data.number->string;
		if (strchr(s, '.') || strchr(s, 'e') || strchr(s, 'E'))
		{
			if ((v = vkaa_tpool_var_create_by_id(context->tpool, r->typeid.id_float, syntax)))
			{
				result->type = vkaa_parse_rtype_var;
				result->data.var = v;
				return result;
			}
		}
		else
		{
			if ((v = vkaa_tpool_var_create_by_id(context->tpool, r->typeid.id_int, syntax)))
			{
				result->type = vkaa_parse_rtype_var;
				result->data.var = v;
				return result;
			}
		}
	}
	return NULL;
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_keyword(vkaa_parse_s *restrict parse, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_parse_set_type2var(parse, typeid, vkaa_syntax_type_keyword, vkaa_std_type2var_label(string), vkaa_parse_keytype_normal);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_string(vkaa_parse_s *restrict parse, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_parse_set_type2var(parse, typeid, vkaa_syntax_type_string, vkaa_std_type2var_label(string), vkaa_parse_keytype_normal);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_number(vkaa_parse_s *restrict parse, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_parse_set_type2var(parse, typeid, vkaa_syntax_type_number, vkaa_std_type2var_label(number), vkaa_parse_keytype_normal);
}
