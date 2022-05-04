#include "std.parse.h"

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var(vkaa_parse_s *restrict p, vkaa_syntax_type_t type, vkaa_parse_type2var_f parse, vkaa_parse_type2var_type_t type2var_type)
{
	vkaa_parse_type2var_s *restrict r, *rr;
	rr = NULL;
	if ((r = (vkaa_parse_type2var_s *) refer_alloc(sizeof(vkaa_parse_type2var_s))))
	{
		r->parse = parse;
		r->type = type2var_type;
		rr = vkaa_parse_type2var_set(p, type, r);
		refer_free(r);
	}
	return rr;
}

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_with_id(vkaa_parse_s *restrict p, vkaa_syntax_type_t type, vkaa_parse_type2var_f parse, vkaa_parse_type2var_type_t type2var_type, const uintptr_t id[], uintptr_t id_number)
{
	vkaa_std_type2var_with_id_s *restrict r;
	vkaa_parse_type2var_s *rr;
	rr = NULL;
	if ((r = (vkaa_std_type2var_with_id_s *) refer_alloc(sizeof(vkaa_std_type2var_with_id_s) + sizeof(uintptr_t) * id_number)))
	{
		r->type2var.parse = parse;
		r->type2var.type = type2var_type;
		r->id_number = id_number;
		while (id_number)
		{
			--id_number;
			r->id[id_number] = id[id_number];
		}
		rr = vkaa_parse_type2var_set(p, type, &r->type2var);
		refer_free(r);
	}
	return rr;
}
