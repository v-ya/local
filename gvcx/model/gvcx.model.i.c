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

mlog_s* gvcx_model_item_iprint(const gvcx_model_item_s *restrict item, gvcx_log_s *restrict log)
{
	static const char mtc[gvcx_model_type_max] = {
		[gvcx_model_type__unknow]  = '!',
		[gvcx_model_type__any]     = '*',
		[gvcx_model_type__uint]    = 'u',
		[gvcx_model_type__int]     = 'i',
		[gvcx_model_type__float]   = 'f',
		[gvcx_model_type__boolean] = 'b',
		[gvcx_model_type__string]  = 's',
		[gvcx_model_type__data]    = 'd',
		[gvcx_model_type__enum]    = 'e',
		[gvcx_model_type__array]   = 'a',
		[gvcx_model_type__object]  = 'o',
		[gvcx_model_type__preset]  = '.',
	};
	const gvcx_model_type_s *restrict type;
	type = item->type;
	return mlog_printf(log->input, "{%c:%02u:%u} %c%c (%s)",
		mtc[type->type_major < gvcx_model_type_max?type->type_major:gvcx_model_type__unknow],
		type->type_major, type->type_minor,
		"-w"[!!(item->item_flag & gvcx_model_flag__write)],
		"-c"[!!(item->item_flag & gvcx_model_flag__child)],
		type->name);
}
