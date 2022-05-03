#include "std.h"
#include "std.parse.h"

vkaa_parse_s* vkaa_std_create_parse(const vkaa_oplevel_s *restrict oplevel, const vkaa_std_typeid_t *restrict typeid)
{
	vkaa_parse_s *restrict r;
	if ((r = vkaa_parse_alloc()))
	{
		if (
			vkaa_std_parse_set_keyword_var(r)
		) return r;
		refer_free(r);
	}
	return NULL;
}
