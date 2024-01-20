#include "gvcx.model.h"

void gvcx_model_type_free_func(gvcx_model_type_s *restrict r)
{
	if (r->tname) refer_free(r->tname);
}

const gvcx_model_type_s* gvcx_model_type_alloc(const char *restrict tname, uintptr_t tsize, gvcx_model_type_initial_f initial, const void *restrict pri)
{
	gvcx_model_type_s *restrict r;
	if (tsize >= sizeof(gvcx_model_type_s) && tname && (r = (gvcx_model_type_s *) refer_alloz(tsize)))
	{
		refer_set_free(r, (refer_free_f) gvcx_model_type_free_func);
		if ((r->tname = refer_dump_string(tname)) && (!initial || initial(r, pri)))
			return r;
		refer_free(r);
	}
	return NULL;
}
