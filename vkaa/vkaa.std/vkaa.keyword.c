#include "std.parse.h"

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword(vkaa_parse_s *restrict p, const char *restrict keyword, vkaa_parse_keyword_f parse, vkaa_parse_keytype_t keytype)
{
	vkaa_parse_keyword_s *restrict r, *rr;
	rr = NULL;
	if ((r = (vkaa_parse_keyword_s *) refer_alloc(sizeof(vkaa_parse_keyword_s))))
	{
		r->parse = parse;
		r->keytype = keytype;
		rr = vkaa_parse_keyword_set(p, keyword, r);
		refer_free(r);
	}
	return rr;
}
