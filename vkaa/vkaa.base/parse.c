#include "../vkaa.parse.h"
#include "../vkaa.syntax.h"

static void vkaa_parse_hashmap_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value) refer_free(vl->value);
}

static void vkaa_parse_free_func(vkaa_parse_s *restrict r)
{
	hashmap_uini(&r->keyword);
	hashmap_uini(&r->operator);
	hashmap_uini(&r->type2var);
}

vkaa_parse_s* vkaa_parse_alloc(void)
{
	vkaa_parse_s *restrict r;
	if ((r = (vkaa_parse_s *) refer_alloz(sizeof(vkaa_parse_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_parse_free_func);
		if (hashmap_init(&r->keyword) && hashmap_init(&r->operator) && hashmap_init(&r->type2var))
			return r;
		refer_free(r);
	}
	return NULL;
}

vkaa_parse_keyword_s* vkaa_parse_keyword_get(const vkaa_parse_s *restrict parse, const char *restrict keyword)
{
	return (vkaa_parse_keyword_s *) hashmap_get_name(&parse->keyword, keyword);
}

vkaa_parse_keyword_s* vkaa_parse_keyword_set(vkaa_parse_s *restrict parse, const char *restrict keyword, vkaa_parse_keyword_s *restrict kw)
{
	if (hashmap_set_name(&parse->keyword, keyword, kw, vkaa_parse_hashmap_free_func))
		return (vkaa_parse_keyword_s *) refer_save(kw);
	return NULL;
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
	if (hashmap_set_name(&parse->operator, operator, op, vkaa_parse_hashmap_free_func))
		return (vkaa_parse_operator_s *) refer_save(op);
	return NULL;
}

void vkaa_parse_operator_unset(vkaa_parse_s *restrict parse, const char *restrict operator)
{
	hashmap_delete_name(&parse->operator, operator);
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
