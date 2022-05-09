#include "std.tpool.h"

static vkaa_std_type_create_define(null)
{
	vkaa_var_s *restrict r;
	if (!syntax && (r = (vkaa_var_s *) refer_alloz(sizeof(vkaa_var_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(r, type))
			return r;
		refer_free(r);
	}
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_null(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "null", typeid->id_null, vkaa_std_type_create_label(null), NULL, typeid);
}
