#include "std.parse.h"

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword(vkaa_parse_s *restrict p, const vkaa_std_typeid_t *restrict typeid, const char *restrict keyword, vkaa_std_keyword_f parse, vkaa_parse_keytype_t keytype)
{
	vkaa_std_keyword_s *restrict r;
	vkaa_parse_keyword_s *rr;
	rr = NULL;
	if ((r = (vkaa_std_keyword_s *) refer_alloc(sizeof(vkaa_std_keyword_s))))
	{
		r->keyword.parse = (vkaa_parse_keyword_f) parse;
		r->keyword.keytype = keytype;
		r->typeid = *typeid;
		rr = vkaa_parse_keyword_set(p, keyword, &r->keyword);
		refer_free(r);
	}
	return rr;
}
