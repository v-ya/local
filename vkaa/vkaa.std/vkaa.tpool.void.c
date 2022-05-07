#include "std.tpool.h"

static vkaa_var_s* vkaa_std_type_void_create(const vkaa_type_s *restrict type)
{
	vkaa_var_s *restrict r;
	if ((r = (vkaa_var_s *) refer_alloz(sizeof(vkaa_var_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(r, type))
			return r;
		refer_free(r);
	}
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_void(vkaa_tpool_s *restrict tpool, const vkaa_std_typeid_t *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "void", typeid->id_void, vkaa_std_type_void_create, NULL, typeid);
}
