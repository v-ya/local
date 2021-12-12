#include "xwin_pool.h"
#include <hashmap.h>

xwin_pool_s *xwin_pool;

hashmap_vlist_t* xwin_pool_parse_initial(void);
void xwin_pool_parse_finally(void);

const char* initial(uintptr_t argc, const char *const argv[])
{
	if ((xwin_pool = xwin_pool_alloc()))
	{
		if (xwin_pool_parse_initial())
			return NULL;
		refer_free(xwin_pool);
	}
	return "xwin.pool.initial";
}

void finally(void)
{
	xwin_pool_parse_finally();
	refer_free(xwin_pool);
}
