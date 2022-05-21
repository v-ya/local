#include "std.tpool.h"

static void vkaa_std_var_syntax_free_func(vkaa_std_var_syntax_s *restrict r)
{
	if (r->syntax) refer_free(r->syntax);
	vkaa_var_finally(&r->var);
}

static vkaa_std_type_create_define(syntax)
{
	vkaa_std_var_syntax_s *restrict r;
	if ((r = (vkaa_std_var_syntax_s *) refer_alloz(sizeof(vkaa_std_var_syntax_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_var_syntax_free_func);
		if (vkaa_var_initial(&r->var, type))
		{
			r->syntax = (const vkaa_syntax_s *) refer_save(syntax);
			return &r->var;
		}
		refer_free(r);
	}
	return NULL;
}

vkaa_type_s* vkaa_std_tpool_set_syntax(vkaa_tpool_s *restrict tpool, vkaa_std_typeid_s *restrict typeid)
{
	return vkaa_std_tpool_set(tpool, "syntax", typeid->id_syntax, typeid,
		vkaa_std_type_create_label(syntax), NULL);
}
