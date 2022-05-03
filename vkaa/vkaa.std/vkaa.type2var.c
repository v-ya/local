#include "std.parse.h"

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var(vkaa_parse_s *restrict p, vkaa_syntax_type_t type, vkaa_parse_type2var_f parse)
{
	vkaa_parse_type2var_s *restrict r, *rr;
	rr = NULL;
	if ((r = (vkaa_parse_type2var_s *) refer_alloc(sizeof(vkaa_parse_type2var_s))))
	{
		r->parse = parse;
		rr = vkaa_parse_type2var_set(p, type, r);
		refer_free(r);
	}
	return rr;
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_with_id(vkaa_parse_s *restrict p, vkaa_syntax_type_t type, vkaa_parse_type2var_f parse, uintptr_t id)
{
	vkaa_std_type2var_with_id_s *restrict r;
	vkaa_parse_type2var_s *rr;
	rr = NULL;
	if ((r = (vkaa_std_type2var_with_id_s *) refer_alloc(sizeof(vkaa_std_type2var_with_id_s))))
	{
		r->type2var.parse = parse;
		r->id = id;
		rr = vkaa_parse_type2var_set(p, type, &r->type2var);
		refer_free(r);
	}
	return rr;
}
