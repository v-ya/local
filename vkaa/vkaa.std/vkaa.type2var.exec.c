#include "std.parse.h"

static vkaa_parse_result_t* vkaa_std_type2var_exec_orginal_scope(const vkaa_parse_type2var_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax)
{
	const vkaa_syntax_s *restrict s;
	vkaa_var_s *restrict var;
	switch (syntax->type)
	{
		case vkaa_syntax_type_scope:
		case vkaa_syntax_type_brackets:
		case vkaa_syntax_type_square:
			s = syntax->data.scope;
			if (vkaa_parse_parse(context, s->syntax_array, s->syntax_number))
			{
				vkaa_execute_set_last(context->execute, NULL);
				if ((var = (vkaa_var_s *) refer_save(context->execute->last_var)) || (((vkaa_std_type2var_with_id_s *) r)->id_number &&
					(var = vkaa_tpool_var_create_by_id(context->tpool, ((vkaa_std_type2var_with_id_s *) r)->id[0], context->scope))))
				{
					result->type = vkaa_parse_rtype_var;
					result->data.var = var;
					result->this = (vkaa_var_s *) refer_save(context->this);
					return result;
				}
			}
			// fall through
		default: return NULL;
	}
}

static vkaa_parse_result_t* vkaa_std_type2var_exec_new_scope(const vkaa_parse_type2var_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax)
{
	vkaa_parse_context_t c;
	vkaa_std_var_scope_s *restrict v;
	vkaa_parse_result_t *rr;
	uintptr_t id_scope;
	rr = NULL;
	id_scope = ((vkaa_std_type2var_with_id_s *) r)->id[1];
	if ((v = (vkaa_std_var_scope_s *) vkaa_tpool_var_create_by_id(context->tpool, id_scope, context->scope)))
	{
		c = *context;
		c.scope = v->scope;
		c.this = &v->var;
		rr = vkaa_std_type2var_exec_orginal_scope(r, result, &c, syntax);
		refer_free(v);
	}
	return rr;
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_scope(vkaa_parse_s *restrict parse, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_parse_set_type2var_with_id(
		parse, vkaa_syntax_type_scope,
		vkaa_std_type2var_exec_new_scope,
		vkaa_parse_keytype_first_allow_ignore,
		(const uintptr_t[]) {typeid->id_void, typeid->id_scope}, 2);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_brackets(vkaa_parse_s *restrict parse)
{
	return vkaa_std_parse_set_type2var_with_id(
		parse, vkaa_syntax_type_brackets,
		vkaa_std_type2var_exec_orginal_scope,
		vkaa_parse_keytype_normal, NULL, 0);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_square(vkaa_parse_s *restrict parse)
{
	return vkaa_std_parse_set_type2var_with_id(
		parse, vkaa_syntax_type_square,
		vkaa_std_type2var_exec_orginal_scope,
		vkaa_parse_keytype_normal, NULL, 0);
}
