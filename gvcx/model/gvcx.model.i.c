#include "gvcx.model.h"

void gvcx_model_item_free_func(gvcx_model_item_s *restrict r)
{
	if (r->tname) refer_free(r->tname);
}

gvcx_model_item_s* gvcx_model_item_alloc(uintptr_t isize, gvcx_model_type_t type, gvcx_model_flag_t flag, refer_string_t tname)
{
	gvcx_model_item_s *restrict r;
	if (isize >= sizeof(gvcx_model_item_s) && tname && (r = (gvcx_model_item_s *) refer_alloz(isize)))
	{
		refer_set_free(r, (refer_free_f) gvcx_model_item_free_func);
		r->type = type;
		r->flag = flag;
		r->tname = (refer_string_t) refer_save(tname);
		return r;
	}
	return NULL;
}
