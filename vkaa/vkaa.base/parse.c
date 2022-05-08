#include "../vkaa.parse.h"
#include "../vkaa.syntax.h"
#include "../vkaa.function.h"
#include "../vkaa.var.h"

static void vkaa_parse_hashmap_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value) refer_free(vl->value);
}

static void vkaa_parse_free_func(vkaa_parse_s *restrict r)
{
	hashmap_uini(&r->keyword);
	hashmap_uini(&r->operator);
	hashmap_uini(&r->op1unary);
	hashmap_uini(&r->type2var);
}

vkaa_parse_s* vkaa_parse_alloc(void)
{
	vkaa_parse_s *restrict r;
	if ((r = (vkaa_parse_s *) refer_alloz(sizeof(vkaa_parse_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_parse_free_func);
		if (hashmap_init(&r->keyword) &&
			hashmap_init(&r->operator) &&
			hashmap_init(&r->op1unary) &&
			hashmap_init(&r->type2var))
			return r;
		refer_free(r);
	}
	return NULL;
}

static refer_t vkaa_parse_refer_set_by_name(hashmap_t *restrict hm, const char *restrict name, refer_t value)
{
	if (hashmap_set_name(hm, name, value, vkaa_parse_hashmap_free_func))
		return refer_save(value);
	return NULL;
}

vkaa_parse_keyword_s* vkaa_parse_keyword_get(const vkaa_parse_s *restrict parse, const char *restrict keyword)
{
	return (vkaa_parse_keyword_s *) hashmap_get_name(&parse->keyword, keyword);
}

vkaa_parse_keyword_s* vkaa_parse_keyword_set(vkaa_parse_s *restrict parse, const char *restrict keyword, vkaa_parse_keyword_s *restrict kw)
{
	return (vkaa_parse_keyword_s *) vkaa_parse_refer_set_by_name(&parse->keyword, keyword, kw);
}

void vkaa_parse_keyword_unset(vkaa_parse_s *restrict parse, const char *restrict keyword)
{
	hashmap_delete_name(&parse->keyword, keyword);
}

vkaa_parse_operator_s* vkaa_parse_operator_get(const vkaa_parse_s *restrict parse, const char *restrict operator)
{
	return (vkaa_parse_operator_s *) hashmap_get_name(&parse->operator, operator);
}

vkaa_parse_operator_s* vkaa_parse_operator_set(vkaa_parse_s *restrict parse, const char *restrict operator, vkaa_parse_operator_s *restrict op)
{
	return (vkaa_parse_operator_s *) vkaa_parse_refer_set_by_name(&parse->operator, operator, op);
}

void vkaa_parse_operator_unset(vkaa_parse_s *restrict parse, const char *restrict operator)
{
	hashmap_delete_name(&parse->operator, operator);
}

vkaa_parse_operator_s* vkaa_parse_op1unary_get(const vkaa_parse_s *restrict parse, const char *restrict operator)
{
	return (vkaa_parse_operator_s *) hashmap_get_name(&parse->op1unary, operator);
}

vkaa_parse_operator_s* vkaa_parse_op1unary_set(vkaa_parse_s *restrict parse, const char *restrict operator, vkaa_parse_operator_s *restrict op)
{
	return (vkaa_parse_operator_s *) vkaa_parse_refer_set_by_name(&parse->op1unary, operator, op);
}

void vkaa_parse_op1unary_unset(vkaa_parse_s *restrict parse, const char *restrict operator)
{
	hashmap_delete_name(&parse->op1unary, operator);
}

vkaa_parse_type2var_s* vkaa_parse_type2var_get(const vkaa_parse_s *restrict parse, vkaa_syntax_type_t type)
{
	return (vkaa_parse_type2var_s *) hashmap_get_head(&parse->type2var, (uint64_t) type);
}

vkaa_parse_type2var_s* vkaa_parse_type2var_set(vkaa_parse_s *restrict parse, vkaa_syntax_type_t type, vkaa_parse_type2var_s *restrict t2v)
{
	if (hashmap_set_head(&parse->type2var, (uint64_t) type, t2v, vkaa_parse_hashmap_free_func))
		return (vkaa_parse_type2var_s *) refer_save(t2v);
	return NULL;
}

void vkaa_parse_type2var_unset(vkaa_parse_s *restrict parse, vkaa_syntax_type_t type)
{
	hashmap_delete_head(&parse->type2var, (uint64_t) type);
}

const vkaa_syntax_t* vkaa_parse_syntax_fetch_and_next(vkaa_parse_syntax_t *restrict syntax)
{
	if (syntax->pos < syntax->number)
		return &syntax->syntax[syntax->pos++];
	return NULL;
}

const vkaa_syntax_t* vkaa_parse_syntax_next_and_fetch(vkaa_parse_syntax_t *restrict syntax)
{
	if (syntax->pos < syntax->number)
	{
		if (++syntax->pos < syntax->number)
			return &syntax->syntax[syntax->pos];
	}
	return NULL;
}

const vkaa_syntax_t* vkaa_parse_syntax_get_curr(vkaa_parse_syntax_t *restrict syntax)
{
	if (syntax->pos < syntax->number)
		return &syntax->syntax[syntax->pos];
	return NULL;
}

const vkaa_syntax_t* vkaa_parse_syntax_get_last(vkaa_parse_syntax_t *restrict syntax)
{
	if (syntax->pos)
		return &syntax->syntax[syntax->pos - 1];
	return NULL;
}

const vkaa_syntax_t* vkaa_parse_syntax_get_range(vkaa_parse_syntax_t *restrict syntax, uintptr_t *restrict number, vkaa_syntax_type_t tail_type, const char *restrict tail_string)
{
	const vkaa_syntax_t *restrict s;
	uintptr_t i, n;
	s = syntax->syntax + syntax->pos;
	n = syntax->number - syntax->pos;
	for (i = 0; i < n; ++i)
	{
		if (vkaa_syntax_test(s + i, tail_type, tail_string))
			goto label_okay;
	}
	if (number) *number = 0;
	return NULL;
	label_okay:
	if (number) *number = i + 1;
	return s;
}

void vkaa_parse_operator_finally(vkaa_parse_operator_s *restrict operator)
{
	if (operator->oplevel) refer_free(operator->oplevel);
}

void vkaa_parse_result_initial(vkaa_parse_result_t *restrict result)
{
	result->type = vkaa_parse_rtype_none;
	result->data.none = NULL;
	result->this = NULL;
}

void vkaa_parse_result_clear(vkaa_parse_result_t *restrict result)
{
	if (result->data.none) refer_free(result->data.none);
	if (result->this) refer_free(result->this);
	result->type = vkaa_parse_rtype_none;
	result->data.none = NULL;
	result->this = NULL;
}

void vkaa_parse_result_set(vkaa_parse_result_t *restrict dst, const vkaa_parse_result_t *restrict src)
{
	vkaa_parse_result_clear(dst);
	dst->type = src->type;
	dst->data.none = refer_save(src->data.none);
	dst->this = (vkaa_var_s *) refer_save(src->this);
}

vkaa_var_s* vkaa_parse_result_get_var(const vkaa_parse_result_t *restrict result)
{
	switch (result->type)
	{
		case vkaa_parse_rtype_var: return result->data.var;
		case vkaa_parse_rtype_function: return vkaa_function_okay(result->data.function);
		default: return NULL;
	}
}

const vkaa_type_s* vkaa_parse_result_get_type(const vkaa_parse_result_t *restrict result)
{
	switch (result->type)
	{
		case vkaa_parse_rtype_var: return result->data.var->type;
		case vkaa_parse_rtype_function: return result->data.function->output_type;
		default: return NULL;
	}
}
