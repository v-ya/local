#include "std.h"

static void vkaa_std_free_func(vkaa_std_s *restrict r)
{
	if (r->parse) refer_free(r->parse);
	if (r->tpool) refer_free(r->tpool);
	if (r->oplevel) refer_free(r->oplevel);
}

vkaa_std_s* vkaa_std_alloc(void)
{
	vkaa_std_s *restrict r;
	if ((r = (vkaa_std_s *) refer_alloz(sizeof(vkaa_std_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_std_free_func);
		if ((r->oplevel = vkaa_std_create_oplevel()) &&
			(r->tpool = vkaa_std_create_tpool(&r->typeid)))
			return r;
		refer_free(r);
	}
	return NULL;
}
