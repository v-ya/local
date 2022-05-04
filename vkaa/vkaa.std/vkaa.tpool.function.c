#include "std.tpool.h"

static void vkaa_std_var_function_free_func(vkaa_std_var_function_s *restrict r)
{
	if (r->param) refer_free(r->param);
	if (r->exec) refer_free(r->exec);
	if (r->scope) refer_free(r->scope);
	vkaa_var_finally(&r->var);
}

static vkaa_var_s* vkaa_std_type_function_create(const vkaa_type_s *restrict type)
{
	vkaa_std_var_function_s *restrict r;
	if ((r = (vkaa_std_var_function_s *) refer_alloz(sizeof(vkaa_std_var_function_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_function_free_func);
		if (vkaa_var_initial(&r->var, type, NULL) &&
			(r->scope = vkaa_scope_alloc(NULL)) &&
			(r->exec = vkaa_execute_alloc()))
			return &r->var;
		refer_free(r);
	}
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_function(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "function", typeid->id_function, vkaa_std_type_function_create);
}
