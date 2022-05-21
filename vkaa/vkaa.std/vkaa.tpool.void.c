#include "std.tpool.h"

static vkaa_std_type_create_define(void)
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

static vkaa_std_type_init_define(void)
{
	if (
		vkaa_std_type_set_function_si(type, "=", sfsi_need2m(void, op_mov)) &&
	1) return type;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_void(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "void", typeid->id_void, typeid,
		vkaa_std_type_create_label(void), vkaa_std_type_init_label(void));
}
