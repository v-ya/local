#include "largeN.h"

static void largeN_cache_free_func(largeN_cache_s *restrict r)
{
	exbuffer_uini(&r->c);
}

largeN_cache_s* largeN_cache_alloc(void)
{
	largeN_cache_s *restrict r;
	if ((r = (largeN_cache_s *) refer_alloc(sizeof(largeN_cache_s))))
	{
		if (exbuffer_init(&r->c, 0))
		{
			refer_set_free(r, (refer_free_f) largeN_cache_free_func);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}
