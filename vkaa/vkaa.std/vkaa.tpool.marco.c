#include "std.tpool.h"

static const vkaa_syntax_s* vkaa_std_var_marco_get_number(const vkaa_syntax_s *restrict syntax_brackets, uintptr_t *restrict rn)
{
	const vkaa_syntax_t *restrict s;
	uintptr_t i, n, number;
	s = syntax_brackets->syntax_array;
	n = syntax_brackets->syntax_number;
	number = i = 0;
	while (i < n)
	{
		if (s[i].type != vkaa_syntax_type_keyword)
			goto label_fail;
		number += 1;
		if (++i >= n)
			break;
		if (s[i].type != vkaa_syntax_type_comma)
			goto label_fail;
		if (++i >= n)
			goto label_fail;
	}
	if (rn) *rn = number;
	return syntax_brackets;
	label_fail:
	if (rn) *rn = 0;
	return NULL;
}

static vkaa_std_var_marco_s* vkaa_std_var_marco_initial(vkaa_std_var_marco_s *restrict r, const vkaa_syntax_s *restrict syntax_brackets)
{
	refer_nstring_t *restrict name;
	const vkaa_syntax_t *restrict s;
	uintptr_t i, n, p;
	name = r->name;
	s = syntax_brackets->syntax_array;
	n = syntax_brackets->syntax_number;
	i = p = 0;
	while (i < n)
	{
		name[p++] = (refer_nstring_t) refer_save(s[i++].data.keyword);
		++i;
	}
	if (p == r->number)
		return r;
	return NULL;
}

static void vkaa_std_var_marco_free_func(vkaa_std_var_marco_s *restrict r)
{
	refer_nstring_t *restrict name;
	uintptr_t i, n;
	name = r->name;
	for (i = 0, n = r->number; i < n; ++i)
	{
		if (name[i]) refer_free(name[i]);
	}
	if (r->scope) refer_free(r->scope);
	vkaa_var_finally(&r->var);
}

static vkaa_std_type_create_define(marco)
{
	vkaa_std_var_marco_s *restrict r;
	uintptr_t number;
	if (syntax && vkaa_std_var_marco_get_number(syntax, &number) &&
		(r = (vkaa_std_var_marco_s *) refer_alloz(sizeof(vkaa_std_var_marco_s) + sizeof(refer_nstring_t) * number)))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_marco_free_func);
		r->number = number;
		if (vkaa_std_var_marco_initial(r, syntax) &&
			vkaa_var_initial(&r->var, type))
			return &r->var;
		refer_free(r);
	}
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_marco(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "marco", typeid->id_marco, vkaa_std_type_create_label(marco), NULL, typeid);
}
