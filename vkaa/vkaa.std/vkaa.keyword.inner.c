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

static vkaa_std_keyword_define(refer_type)
{
	vkaa_parse_result_t ret;
	const vkaa_type_s *restrict type;
	const vkaa_syntax_t *restrict s;
	vkaa_var_s *restrict var;
	vkaa_parse_result_initial(&ret);
	if (!(type = vkaa_std_keyword_parse_get_type(context->tpool, context->scope, syntax)))
		goto label_fail;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)) || s->type != vkaa_syntax_type_brackets)
		goto label_fail;
	if (!vkaa_parse_parse(context, s, 1, &ret))
		goto label_fail;
	if (!(var = vkaa_parse_result_get_var(&ret)))
		goto label_fail;
	if (var->type_id != r->typeid->id_refer)
		goto label_fail;
	if (!vkaa_std_var_refer_must_type((vkaa_std_var_refer_s *) var, type))
		goto label_fail;
	result->type = vkaa_parse_rtype_var;
	result->data.var = (vkaa_var_s *) refer_save(var);
	vkaa_parse_result_clear(&ret);
	return result;
	label_fail:
	vkaa_parse_result_clear(&ret);
	return NULL;
}

static vkaa_std_keyword_define(op_refer)
{
	vkaa_parse_result_t ret;
	const vkaa_type_s *restrict type;
	vkaa_std_var_refer_s *restrict var;
	vkaa_var_s *restrict refer_var;
	const vkaa_syntax_t *restrict s;
	var = NULL;
	vkaa_parse_result_initial(&ret);
	if (!(type = vkaa_tpool_find_id(context->tpool, r->typeid->id_refer)))
		goto label_fail;
	if (!(s = vkaa_parse_syntax_fetch_and_next(syntax)))
		goto label_fail;
	if (!vkaa_parse_parse(context, s, 1, &ret))
		goto label_fail;
	if (!(refer_var = vkaa_parse_result_get_var(&ret)))
		goto label_fail;
	if (!(var = vkaa_std_type_refer_create_by_type(type, refer_var->type)))
		goto label_fail;
	if (!vkaa_std_var_refer_link(var, refer_var))
		goto label_fail;
	result->type = vkaa_parse_rtype_var;
	result->data.var = &var->var;
	vkaa_parse_result_clear(&ret);
	return result;
	label_fail:
	if (var) refer_free(var);
	vkaa_parse_result_clear(&ret);
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

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_refer_type(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "refer_type", vkaa_std_keyword_label(refer_type), vkaa_parse_keytype_inner);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_op_refer(vkaa_parse_s *restrict parse, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_parse_set_keyword(parse, typeid, "&", vkaa_std_keyword_label(op_refer), vkaa_parse_keytype_inner);
}
