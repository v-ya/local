#include "../vkaa.scope.h"

static void vkaa_scope_free_func(vkaa_scope_s *restrict r)
{
	if (r->parent) refer_free(r->parent);
	hashmap_uini(&r->var);
}

vkaa_scope_s* vkaa_scope_alloc(vkaa_scope_s *restrict parent)
{
	vkaa_scope_s *restrict r;
	if ((r = (vkaa_scope_s *) refer_alloz(sizeof(vkaa_scope_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_scope_free_func);
		if ((r->parent = (vkaa_scope_s *) refer_save(parent)))
			r->level = parent->level + 1;
		if (hashmap_init(&r->var))
			return r;
		refer_free(r);
	}
	return NULL;
}

vkaa_var_s* vkaa_scope_find(const vkaa_scope_s *restrict scope, const char *restrict name)
{
	vkaa_var_s *restrict var;
	var = NULL;
	while (scope && !(var = (vkaa_var_s *) hashmap_get_name(&scope->var, name)))
		scope = scope->parent;
	return var;
}

static void vkaa_scope_hashmap_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value) refer_free(vl->value);
}

vkaa_var_s* vkaa_scope_put(vkaa_scope_s *restrict scope, const char *restrict name, vkaa_var_s *var)
{
	if (!hashmap_find_name(&scope->var, name) && hashmap_set_name(&scope->var, name, var, vkaa_scope_hashmap_free_func))
	{
		refer_save(var);
		return var;
	}
	return NULL;
}

vkaa_var_s* vkaa_scope_set(vkaa_scope_s *restrict scope, const char *restrict name, vkaa_var_s *var)
{
	if (hashmap_set_name(&scope->var, name, var, vkaa_scope_hashmap_free_func))
	{
		refer_save(var);
		return var;
	}
	return NULL;
}

void vkaa_scope_unset(vkaa_scope_s *restrict scope, const char *restrict name)
{
	hashmap_delete_name(&scope->var, name);
}
