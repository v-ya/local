#include "gvcx.model.h"

void gvcx_model_item_free_func(gvcx_model_item_s *restrict r)
{
	if (r->type) refer_free(r->type);
}

gvcx_model_item_s* gvcx_model_item_alloc(uintptr_t isize, const gvcx_model_type_s *restrict type)
{
	gvcx_model_item_s *restrict r;
	if (isize >= sizeof(gvcx_model_item_s) && type && (r = (gvcx_model_item_s *) refer_alloz(isize)))
	{
		refer_set_free(r, (refer_free_f) gvcx_model_item_free_func);
		r->type = (const gvcx_model_type_s *) refer_save(type);
		r->type_major = type->type_major;
		r->item_flag = gvcx_model_flag__write;
		return r;
	}
	return NULL;
}
