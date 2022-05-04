#include "std.tpool.h"

static vkaa_var_s* vkaa_std_type_int_create(const vkaa_type_s *restrict type)
{
	vkaa_std_var_int_s *restrict r;
	if ((r = (vkaa_std_var_int_s *) refer_alloz(sizeof(vkaa_std_var_int_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(&r->var, type, &r->value))
			return &r->var;
		refer_free(r);
	}
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_int(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "int", typeid->id_int, vkaa_std_type_int_create);
}
