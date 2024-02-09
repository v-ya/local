#include "layer.model.h"

void layer_model_item_free_func(layer_model_item_s *restrict r)
{
	if (r->type) refer_free(r->type);
}

layer_model_item_s* layer_model_item_alloc(uintptr_t isize, const layer_model_type_s *restrict type)
{
	layer_model_item_s *restrict r;
	if (isize >= sizeof(layer_model_item_s) && type && (r = (layer_model_item_s *) refer_alloz(isize)))
	{
		refer_set_free(r, (refer_free_f) layer_model_item_free_func);
		r->type = (const layer_model_type_s *) refer_save(type);
		r->type_major = type->type_major;
		r->item_flag = layer_model_flag__write;
		return r;
	}
	return NULL;
}

mlog_s* layer_model_item_iprint(const layer_model_item_s *restrict item, layer_log_s *restrict log)
{
	static const char mtc[layer_model_type_max] = {
		[layer_model_type__unknow]  = '!',
		[layer_model_type__any]     = '*',
		[layer_model_type__uint]    = 'u',
		[layer_model_type__int]     = 'i',
		[layer_model_type__float]   = 'f',
		[layer_model_type__boolean] = 'b',
		[layer_model_type__string]  = 's',
		[layer_model_type__data]    = 'd',
		[layer_model_type__enum]    = 'e',
		[layer_model_type__array]   = 'a',
		[layer_model_type__object]  = 'o',
		[layer_model_type__preset]  = '.',
	};
	const layer_model_type_s *restrict type;
	type = item->type;
	return mlog_printf(log->input, "{%c:%02u:%u} %c%c (%s)",
		mtc[type->type_major < layer_model_type_max?type->type_major:layer_model_type__unknow],
		type->type_major, type->type_minor,
		"-w"[!!(item->item_flag & layer_model_flag__write)],
		"-c"[!!(item->item_flag & layer_model_flag__child)],
		type->name);
}
