#include "std.parse.h"

static void vkaa_std_keyword_free_func(vkaa_std_keyword_s *restrict r)
{
	if (r->typeid) refer_free(r->typeid);
}

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword(vkaa_parse_s *restrict p, vkaa_std_typeid_s *restrict typeid, const char *restrict keyword, vkaa_std_keyword_f parse, vkaa_parse_keytype_t keytype)
{
	vkaa_std_keyword_s *restrict r;
	vkaa_parse_keyword_s *rr;
	rr = NULL;
	if ((r = (vkaa_std_keyword_s *) refer_alloc(sizeof(vkaa_std_keyword_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_keyword_free_func);
		r->keyword.parse = (vkaa_parse_keyword_f) parse;
		r->keyword.keytype = keytype;
		r->typeid = (vkaa_std_typeid_s *) refer_save(typeid);
		rr = vkaa_parse_keyword_set(p, keyword, &r->keyword);
		refer_free(r);
	}
	return rr;
}
