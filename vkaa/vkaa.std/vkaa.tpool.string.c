#include "std.tpool.h"

static void vkaa_std_var_string_free_func(vkaa_std_var_string_s *restrict r)
{
	if (r->value) refer_free(r->value);
	vkaa_var_finally(&r->var);
}

static vkaa_std_type_create_define(string)
{
	vkaa_std_var_string_s *restrict r;
	if ((r = (vkaa_std_var_string_s *) refer_alloz(sizeof(vkaa_std_var_string_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_string_free_func);
		if (vkaa_var_initial(&r->var, type))
		{
			if (syntax)
			{
				if (syntax->type == vkaa_syntax_type_keyword ||
					syntax->type == vkaa_syntax_type_string)
					r->value = (refer_nstring_t) refer_save(syntax->data.string);
				else goto label_fail;
			}
			return &r->var;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_string(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "string", typeid->id_string, vkaa_std_type_create_label(string), NULL, typeid);
}
