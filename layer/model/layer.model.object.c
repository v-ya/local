#include "layer.model.h"
#include <hashmap.h>
#include <exbuffer.h>

typedef struct layer_model_object_item_s layer_model_object_item_s;

struct layer_model_object_item_s {
	refer_string_t name;
	const layer_model_any_s *any;
	layer_model_item_s *dv;
};

struct layer_model_object_s {
	vattr_s param;
};

static void layer_model_object_item_free_func(layer_model_object_item_s *restrict r)
{
	if (r->dv) refer_free(r->dv);
	if (r->any) refer_free(r->any);
	if (r->name) refer_free(r->name);
}
static layer_model_object_item_s* layer_model_object_item_alloc(refer_string_t name, const layer_model_any_s *restrict any, layer_model_item_s *restrict dv)
{
	layer_model_object_item_s *restrict r;
	if ((!name || layer_model_type_name_enum(name) != layer_model_type__unknow) &&
		(!dv || !(dv->item_flag & layer_model_flag__child)) &&
		(r = (layer_model_object_item_s *) refer_alloz(sizeof(layer_model_object_item_s))))
	{
		refer_set_free(r, (refer_free_f) layer_model_object_item_free_func);
		r->name = (refer_string_t) refer_save(name);
		r->any = (const layer_model_any_s *) refer_save(any);
		r->dv = (layer_model_item_s *) refer_save(dv);
		if (dv)
		{
			dv->item_flag &= ~(uint32_t) layer_model_flag__write;
			dv->item_flag |= (uint32_t) layer_model_flag__child;
		}
		return r;
	}
	return NULL;
}

layer_model_object_s* layer_model_object_alloc(void)
{
	return (layer_model_object_s *) vattr_alloc();
}

const vattr_s* layer_model_object_vattr(const layer_model_object_s *restrict object)
{
	return &object->param;
}

layer_model_object_s* layer_model_object_insert(layer_model_object_s *restrict object, const char *restrict key, refer_string_t name, const layer_model_any_s *restrict any, layer_model_item_s *restrict dv)
{
	const layer_model_object_item_s *restrict item;
	if (key && !vattr_get_vslot(&object->param, key) &&
		(item = layer_model_object_item_alloc(name, any, dv)))
	{
		if (!vattr_insert_tail(&object->param, key, item))
			object = NULL;
		refer_free(item);
		return object;
	}
	return NULL;
}

const layer_model_object_s* layer_model_object_find(const layer_model_object_s *restrict object, const char *restrict key, refer_string_t *restrict name, const layer_model_any_s *restrict *restrict any, layer_model_item_s *restrict *restrict dv)
{
	const layer_model_object_item_s *restrict item;
	if (key && (item = (const layer_model_object_item_s *) vattr_get_first(&object->param, key)))
	{
		if (name) *name = item->name;
		if (any) *any = item->any;
		if (dv) *dv = item->dv;
		return object;
	}
	if (name) *name = NULL;
	if (any) *any = NULL;
	if (dv) *dv = NULL;
	return NULL;
}
