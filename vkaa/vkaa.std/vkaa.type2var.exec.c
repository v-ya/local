#include "std.parse.h"

static vkaa_var_s* vkaa_std_type2var_exec_function(const vkaa_function_s *restrict r)
{
	return r->output;
}

static vkaa_parse_result_t* vkaa_std_type2var_exec_orginal_scope(const vkaa_parse_type2var_s *restrict r, vkaa_parse_result_t *restrict result, const vkaa_parse_context_t *restrict context, const vkaa_syntax_t *restrict syntax)
{
	const vkaa_syntax_s *restrict s;
	vkaa_var_s *restrict var;
	vkaa_selector_param_t param;
	vkaa_selector_rdata_t rdata;
	switch (syntax->type)
	{
		case vkaa_syntax_type_scope:
		case vkaa_syntax_type_brackets:
		case vkaa_syntax_type_square:
			s = syntax->data.scope;
			if (vkaa_parse_parse(context, s->syntax_array, s->syntax_number))
			{
				vkaa_execute_set_last(context->execute, NULL);
				if ((var = (vkaa_var_s *) refer_save(context->execute->last_var)) || (((vkaa_std_type2var_with_id_s *) r)->id &&
					(var = vkaa_tpool_var_create_by_id(context->tpool, ((vkaa_std_type2var_with_id_s *) r)->id, context->scope))))
				{
					param.name = NULL;
					param.exec = context->execute;
					param.this = context->this;
					param.input_list = NULL;
					param.input_number = 0;
					rdata.function = vkaa_std_type2var_exec_function;
					rdata.output_typeid = var->type_id;
					rdata.output_must = var;
					if ((result->data.function = vkaa_function_alloc(NULL, &param, &rdata)))
					{
						result->type = vkaa_parse_rtype_function;
						result->this = (vkaa_var_s *) refer_save(context->this);
					}
					refer_free(var);
				}
			}
			return result->type?result:NULL;
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
	id_scope = ((vkaa_std_type2var_with_id2_s *) r)->id2;
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
	return vkaa_std_parse_set_type2var_with_id2(parse, vkaa_syntax_type_scope, vkaa_std_type2var_exec_new_scope, vkaa_parse_keytype_first_allow_ignore, typeid->id_void, typeid->id_scope);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_brackets(vkaa_parse_s *restrict parse)
{
	return vkaa_std_parse_set_type2var_with_id(parse, vkaa_syntax_type_brackets, vkaa_std_type2var_exec_orginal_scope, vkaa_parse_keytype_normal, 0);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_square(vkaa_parse_s *restrict parse)
{
	return vkaa_std_parse_set_type2var_with_id(parse, vkaa_syntax_type_square, vkaa_std_type2var_exec_orginal_scope, vkaa_parse_keytype_normal, 0);
}
