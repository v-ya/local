#include "std.parse.h"
#include <string.h>

static vkaa_std_type2var_define(string)
{
	const vkaa_type_s *restrict type;
	vkaa_var_s *restrict v;
	switch (syntax->type)
	{
		case vkaa_syntax_type_keyword:
		case vkaa_syntax_type_string:
			if ((type = vkaa_tpool_find_id(context->tpool, r->typeid->id_string)) &&
				(v = &vkaa_std_type_string_create_by_value(type, syntax->data.string)->var))
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
	const vkaa_type_s *restrict type;
	vkaa_var_s *restrict v;
	const char *restrict s;
	if (syntax->type == vkaa_syntax_type_number)
	{
		s = syntax->data.number->string;
		if (strchr(s, '.') || strchr(s, 'e') || strchr(s, 'E'))
		{
			double value;
			if (vkaa_syntax_convert_double(syntax, &value) &&
				(type = vkaa_tpool_find_id(context->tpool, r->typeid->id_float)) &&
				(v = &vkaa_std_type_float_create_by_value(type, value)->var))
				goto label_okay;
		}
		else
		{
			intptr_t value;
			if (vkaa_syntax_convert_intptr(syntax, &value, 0) &&
				(type = vkaa_tpool_find_id(context->tpool, r->typeid->id_int)) &&
				(v = &vkaa_std_type_int_create_by_value(type, value)->var))
				goto label_okay;
		}
	}
	return NULL;
	label_okay:
	result->type = vkaa_parse_rtype_var;
	result->data.var = v;
	return result;
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_keyword(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_type2var(parse, typeid, vkaa_syntax_type_keyword, vkaa_std_type2var_label(string), vkaa_parse_keytype_normal);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_string(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_type2var(parse, typeid, vkaa_syntax_type_string, vkaa_std_type2var_label(string), vkaa_parse_keytype_normal);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_number(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_type2var(parse, typeid, vkaa_syntax_type_number, vkaa_std_type2var_label(number), vkaa_parse_keytype_normal);
}
