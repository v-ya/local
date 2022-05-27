#include "std.tpool.h"

static void vkaa_std_var_refer_free_func(vkaa_std_var_refer_s *restrict r)
{
	if (r->refer_type) refer_free(r->refer_type);
	if (r->refer_var) refer_free(r->refer_var);
	vkaa_var_finally(&r->var);
}

vkaa_std_var_refer_s* vkaa_std_type_refer_create_by_type(const vkaa_type_s *restrict type, const vkaa_type_s *restrict refer_type)
{
	vkaa_std_var_refer_s *restrict r;
	if ((r = (vkaa_std_var_refer_s *) refer_alloz(sizeof(vkaa_std_var_refer_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_refer_free_func);
		if (vkaa_var_initial(&r->var, type))
		{
			r->refer_type = (const vkaa_type_s *) refer_save(refer_type);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static vkaa_std_type_create_define(refer)
{
	if (!syntax)
		return &vkaa_std_type_refer_create_by_type(type, NULL)->var;
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_refer(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "refer", typeid->id_refer, typeid,
		vkaa_std_type_create_label(refer), NULL);
}
