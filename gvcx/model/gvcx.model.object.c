#include "gvcx.model.h"
#include <hashmap.h>
#include <exbuffer.h>

typedef struct gvcx_model_object_item_s gvcx_model_object_item_s;

struct gvcx_model_object_item_s {
	refer_string_t name;
	const gvcx_model_any_s *any;
	gvcx_model_item_s *dv;
};

struct gvcx_model_object_s {
	vattr_s param;
};

static void gvcx_model_object_item_free_func(gvcx_model_object_item_s *restrict r)
{
	if (r->dv) refer_free(r->dv);
	if (r->any) refer_free(r->any);
	if (r->name) refer_free(r->name);
}
static gvcx_model_object_item_s* gvcx_model_object_item_alloc(refer_string_t name, const gvcx_model_any_s *restrict any, gvcx_model_item_s *restrict dv)
{
	gvcx_model_object_item_s *restrict r;
	if ((!name || gvcx_model_type_name_enum(name) != gvcx_model_type__unknow) &&
		(!dv || !(dv->item_flag & gvcx_model_flag__child)) &&
		(r = (gvcx_model_object_item_s *) refer_alloz(sizeof(gvcx_model_object_item_s))))
	{
		refer_set_free(r, (refer_free_f) gvcx_model_object_item_free_func);
		r->name = (refer_string_t) refer_save(name);
		r->any = (const gvcx_model_any_s *) refer_save(any);
		r->dv = (gvcx_model_item_s *) refer_save(dv);
		if (dv)
		{
			dv->item_flag &= ~(uint32_t) gvcx_model_flag__write;
			dv->item_flag |= (uint32_t) gvcx_model_flag__child;
		}
		return r;
	}
	return NULL;
}

gvcx_model_object_s* gvcx_model_object_alloc(void)
{
	return (gvcx_model_object_s *) vattr_alloc();
}

const vattr_s* gvcx_model_object_vattr(const gvcx_model_object_s *restrict object)
{
	return &object->param;
}

gvcx_model_object_s* gvcx_model_object_insert(gvcx_model_object_s *restrict object, const char *restrict key, refer_string_t name, const gvcx_model_any_s *restrict any, gvcx_model_item_s *restrict dv)
{
	const gvcx_model_object_item_s *restrict item;
	if (key && !vattr_get_vslot(&object->param, key) &&
		(item = gvcx_model_object_item_alloc(name, any, dv)))
	{
		if (!vattr_insert_tail(&object->param, key, item))
			object = NULL;
		refer_free(item);
		return object;
	}
	return NULL;
}

const gvcx_model_object_s* gvcx_model_object_find(const gvcx_model_object_s *restrict object, const char *restrict key, refer_string_t *restrict name, const gvcx_model_any_s *restrict *restrict any, gvcx_model_item_s *restrict *restrict dv)
{
	const gvcx_model_object_item_s *restrict item;
	if (key && (item = (const gvcx_model_object_item_s *) vattr_get_first(&object->param, key)))
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
