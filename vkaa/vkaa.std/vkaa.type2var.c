#include "std.parse.h"

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, vkaa_syntax_type_t type, vkaa_std_type2var_f parse, vkaa_parse_type2var_type_t type2var_type)
{
	vkaa_std_type2var_s *restrict r;
	vkaa_parse_type2var_s *rr;
	rr = NULL;
	if ((r = (vkaa_std_type2var_s *) refer_alloc(sizeof(vkaa_std_type2var_s))))
	{
		r->type2var.parse = (vkaa_parse_type2var_f) parse;
		r->type2var.type = type2var_type;
		r->typeid = *typeid;
		rr = vkaa_parse_type2var_set(p, type, &r->type2var);
		refer_free(r);
	}
	return rr;
}
