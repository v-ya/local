#include "std.parse.h"

static void vkaa_std_type2var_free_func(vkaa_std_keyword_s *restrict r)
{
	if (r->typeid) refer_free(r->typeid);
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, vkaa_syntax_type_t type, vkaa_std_type2var_f parse, vkaa_parse_type2var_type_t type2var_type)
{
	vkaa_std_type2var_s *restrict r;
	vkaa_parse_type2var_s *rr;
	rr = NULL;
	if ((r = (vkaa_std_type2var_s *) refer_alloc(sizeof(vkaa_std_type2var_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_type2var_free_func);
		r->type2var.parse = (vkaa_parse_type2var_f) parse;
		r->type2var.type = type2var_type;
		r->typeid = (vkaa_std_typeid_s *) refer_save(typeid);
		rr = vkaa_parse_type2var_set(p, type, &r->type2var);
		refer_free(r);
	}
	return rr;
}
