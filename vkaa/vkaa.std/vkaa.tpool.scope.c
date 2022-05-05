#include "std.tpool.h"

static void vkaa_std_var_scope_free_func(vkaa_std_var_scope_s *restrict r)
{
	if (r->scope) refer_free(r->scope);
	vkaa_var_finally(&r->var);
}

vkaa_std_var_scope_s* vkaa_std_type_scope_create_by_parent(const vkaa_type_s *restrict type, vkaa_scope_s *restrict scope)
{
	vkaa_std_var_scope_s *restrict r;
	if ((r = (vkaa_std_var_scope_s *) refer_alloz(sizeof(vkaa_std_var_scope_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_scope_free_func);
		if (vkaa_var_initial(&r->var, type, NULL) &&
			(r->scope = vkaa_scope_alloc(scope)))
			return r;
		refer_free(r);
	}
	return NULL;
}

static vkaa_var_s* vkaa_std_type_scope_create(const vkaa_type_s *restrict type)
{
	return &vkaa_std_type_scope_create_by_parent(type, NULL)->var;
}

vkaa_type_s* vkaa_std_tpool_set_scope(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "scope", typeid->id_scope, vkaa_std_type_scope_create, NULL, typeid);
}