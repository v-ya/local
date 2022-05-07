#include "std.parse.h"

static vkaa_std_type2var_define(exec_orginal_scope)
{
	const vkaa_syntax_s *restrict s;
	switch (syntax->type)
	{
		case vkaa_syntax_type_scope:
		case vkaa_syntax_type_brackets:
		case vkaa_syntax_type_square:
			s = syntax->data.scope;
			if (vkaa_parse_parse(context, s->syntax_array, s->syntax_number, result))
			{
				if (result->type)
					goto label_okay;
				else if (syntax->type == vkaa_syntax_type_scope &&
					(result->data.var = vkaa_tpool_var_create_by_id(context->tpool, r->typeid.id_void, NULL)))
				{
					result->type = vkaa_parse_rtype_var;
					label_okay:
					return result;
				}
			}
			// fall through
		default: return NULL;
	}
}

static vkaa_std_type2var_define(exec_new_scope)
{
	vkaa_parse_context_t c;
	const vkaa_type_s *restrict type;
	vkaa_std_var_scope_s *restrict v;
	vkaa_parse_result_t *rr;
	rr = NULL;
	if ((type = vkaa_tpool_find_id(context->tpool, r->typeid.id_scope)) &&
		(v = vkaa_std_type_scope_create_by_parent(type, context->scope)))
	{
		c = *context;
		c.scope = v->scope;
		c.this = &v->var;
		rr = vkaa_std_type2var_label(exec_orginal_scope)(r, result, &c, syntax);
		refer_free(v);
	}
	return rr;
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_scope(vkaa_parse_s *restrict parse, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_parse_set_type2var(parse, typeid, vkaa_syntax_type_scope, vkaa_std_type2var_label(exec_new_scope), vkaa_parse_keytype_first_allow_ignore);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_brackets(vkaa_parse_s *restrict parse, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_parse_set_type2var(parse, typeid, vkaa_syntax_type_brackets, vkaa_std_type2var_label(exec_orginal_scope), vkaa_parse_keytype_normal);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_square(vkaa_parse_s *restrict parse, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_parse_set_type2var(parse, typeid, vkaa_syntax_type_square, vkaa_std_type2var_label(exec_orginal_scope), vkaa_parse_keytype_normal);
}
