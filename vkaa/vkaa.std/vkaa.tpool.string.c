#include "std.tpool.h"

static void vkaa_std_var_string_free_func(vkaa_std_var_string_s *restrict r)
{
	if (r->value) refer_free(r->value);
	vkaa_var_finally(&r->var);
}

static vkaa_var_s* vkaa_std_type_string_create(const vkaa_type_s *restrict type)
{
	vkaa_std_var_string_s *restrict r;
	if ((r = (vkaa_std_var_string_s *) refer_alloz(sizeof(vkaa_std_var_string_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_string_free_func);
		if (vkaa_var_initial(&r->var, type))
			return &r->var;
		refer_free(r);
	}
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_string(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "string", typeid->id_string, vkaa_std_type_string_create, NULL, typeid);
}
