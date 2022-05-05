#include "std.parse.h"
#include <string.h>
#include <stdlib.h>

static vkaa_std_type2var_define(string)
{
	vkaa_std_var_string_s *restrict v;
	switch (syntax->type)
	{
		case vkaa_syntax_type_keyword:
		case vkaa_syntax_type_string:
			if ((v = (vkaa_std_var_string_s *) vkaa_tpool_var_create_by_id(context->tpool, r->typeid.id_string)))
			{
				v->value = (refer_nstring_t) refer_save(syntax->data.string);
				result->type = vkaa_parse_rtype_var;
				result->data.var = &v->var;
				return result;
			}
			// fall through
		default: return NULL;
	}
}

static vkaa_std_type2var_define(number)
{
	const char *restrict s;
	char *endptr;
	if (syntax->type == vkaa_syntax_type_number)
	{
		s = syntax->data.number->string;
		if (strchr(s, '.') || strchr(s, 'e') || strchr(s, 'E'))
		{
			vkaa_std_var_float_s *restrict v;
			vkaa_std_float_t vv;
			vv = (vkaa_std_float_t) strtod(s, &endptr);
			if (!*endptr && (v = (vkaa_std_var_float_s *) vkaa_tpool_var_create_by_id(context->tpool, r->typeid.id_float)))
			{
				v->value = vv;
				result->type = vkaa_parse_rtype_var;
				result->data.var = &v->var;
				return result;
			}
		}
		else
		{
			vkaa_std_var_int_s *restrict v;
			vkaa_std_int_t vv;
			vv = (vkaa_std_int_t) strtoll(s, &endptr, 0);
			if (!*endptr && (v = (vkaa_std_var_int_s *) vkaa_tpool_var_create_by_id(context->tpool, r->typeid.id_int)))
			{
				v->value = vv;
				result->type = vkaa_parse_rtype_var;
				result->data.var = &v->var;
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
