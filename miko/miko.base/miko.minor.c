#include "miko.minor.h"

void miko_minor_free_func(miko_minor_s *restrict r)
{
	refer_ck_free(r->name);
}

miko_minor_s* miko_minor_alloc(const char *restrict name, uintptr_t size)
{
	miko_minor_s *restrict r;
	if (name && size >= sizeof(miko_minor_s) && (r = (miko_minor_s *) refer_alloz(size)))
	{
		refer_hook_free(r, minor);
		if ((r->name = refer_dump_string(name)))
			return r;
		refer_free(r);
	}
	return NULL;
}
