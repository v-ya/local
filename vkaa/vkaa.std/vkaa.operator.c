#include "std.parse.h"

vkaa_parse_operator_s* vkaa_std_parse_set_operator(vkaa_parse_s *restrict p, const vkaa_oplevel_s *restrict opl, const char *restrict operator, vkaa_parse_operator_f parse, vkaa_parse_optype_t optype, const char *restrict oplevel, const char *restrict ternary_second_operator)
{
	const vkaa_level_s *restrict level;
	vkaa_parse_operator_s *restrict r, *rr;
	rr = NULL;
	if ((level = vkaa_oplevel_get(opl, oplevel)) &&
		(r = (vkaa_parse_operator_s *) refer_alloc(sizeof(vkaa_parse_operator_s))))
	{
		r->parse = parse;
		r->optype = optype;
		r->oplevel = (const vkaa_level_s *) refer_save(level);
		r->ternary_second_operator = ternary_second_operator;
		rr = vkaa_parse_operator_set(p, operator, r);
		refer_free(r);
	}
	return rr;
}
