#include "std.tpool.h"

static void vkaa_std_var_string_free_func(vkaa_std_var_string_s *restrict r)
{
	if (r->value) refer_free(r->value);
	vkaa_var_finally(&r->var);
}

vkaa_std_var_string_s* vkaa_std_type_string_create_by_value(const vkaa_type_s *restrict type, refer_nstring_t value)
{
	vkaa_std_var_string_s *restrict r;
	if ((r = (vkaa_std_var_string_s *) refer_alloz(sizeof(vkaa_std_var_string_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_string_free_func);
		if (vkaa_var_initial(&r->var, type))
		{
			r->value = (refer_nstring_t) refer_save(value);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static vkaa_std_type_create_define(string)
{
	return &vkaa_std_type_string_create_by_value(type, NULL)->var;
}

vkaa_type_s* vkaa_std_tpool_set_string(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "string", typeid->id_string, vkaa_std_type_create_label(string), NULL, typeid);
}
