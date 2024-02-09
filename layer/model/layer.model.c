#include "layer.model.h"

// pri

const layer_model_type_s* layer_model_find_type(const layer_model_s *restrict m, const char *restrict name)
{
	return (const layer_model_type_s *) vattr_get_first(m->pool_type, name);
}
const layer_model_any_s* layer_model_find_any(const layer_model_s *restrict m, const char *restrict name)
{
	return (const layer_model_any_s *) vattr_get_first(m->pool_any, name);
}
const layer_model_enum_s* layer_model_find_enum(const layer_model_s *restrict m, const char *restrict name)
{
	return (const layer_model_enum_s *) vattr_get_first(m->pool_enum, name);
}
const layer_model_object_s* layer_model_find_object(const layer_model_s *restrict m, const char *restrict name)
{
	return (const layer_model_object_s *) vattr_get_first(m->pool_object, name);
}
refer_string_t layer_model_find_cname(const layer_model_s *restrict m, const char *restrict cname, const layer_model_type_s *restrict *restrict type, const layer_model_any_s *restrict *restrict any)
{
	const layer_model_type_s *restrict t;
	const layer_model_any_s *restrict a;
	if ((t = layer_model_find_type(m, cname)))
	{
		if (type) *type = t;
		if (any) *any = NULL;
		return t->name;
	}
	else if ((a = layer_model_find_any(m, cname)))
	{
		if (type) *type = NULL;
		if (any) *any = a;
		return layer_model_any_name(a);
	}
	else
	{
		if (type) *type = NULL;
		if (any) *any = NULL;
		return NULL;
	}
}

layer_model_s* layer_model_add_type(layer_model_s *restrict m, const layer_model_type_s *restrict type)
{
	if (type && !vattr_get_vslot(m->pool_type, type->name) &&
		vattr_insert_tail(m->pool_type, type->name, type))
		return m;
	return NULL;
}
layer_model_s* layer_model_add_any(layer_model_s *restrict m, const layer_model_any_s *restrict any)
{
	refer_string_t name;
	if (any && (name = layer_model_any_name(any)) &&
		!vattr_get_vslot(m->pool_any, name) &&
		vattr_insert_tail(m->pool_any, name, any))
		return m;
	return NULL;
}
layer_model_s* layer_model_add_enum(layer_model_s *restrict m, const char *restrict name, const layer_model_enum_s *restrict e)
{
	if (name && e && !vattr_get_vslot(m->pool_enum, name) &&
		vattr_insert_tail(m->pool_enum, name, e))
		return m;
	return NULL;
}
layer_model_s* layer_model_add_object(layer_model_s *restrict m, const char *restrict name, const layer_model_object_s *restrict object)
{
	if (name && object && !vattr_get_vslot(m->pool_object, name) &&
		vattr_insert_tail(m->pool_object, name, object))
		return m;
	return NULL;
}

// api

static void layer_model_free_func(refer_t *restrict r)
{
	uintptr_t i, n;
	for (i = 0, n = sizeof(layer_model_s) / sizeof(refer_t); i < n; ++i)
	{
		if (r[i]) refer_free(r[i]);
	}
}

layer_model_s* layer_model_alloc(void)
{
	layer_model_s *restrict r;
	if ((r = (layer_model_s *) refer_alloz(sizeof(layer_model_s))))
	{
		refer_set_free(r, (refer_free_f) layer_model_free_func);
		if (!(r->pool_type = vattr_alloc())) goto label_fail;
		if (!(r->pool_any = vattr_alloc())) goto label_fail;
		if (!(r->pool_enum = vattr_alloc())) goto label_fail;
		if (!(r->pool_object = vattr_alloc())) goto label_fail;
		#define d_type(_t, ...)  if (!layer_model_create_type__##_t(r, layer_model_stype__##_t, 0, ##__VA_ARGS__)) goto label_fail
		d_type(null);
		d_type(uint);
		d_type(int);
		d_type(float);
		d_type(boolean);
		d_type(string);
		d_type(data);
		d_type(array, NULL);
		d_type(object, NULL, NULL);
		#undef d_type
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

layer_model_item_s* layer_model_create_item(const layer_model_s *restrict m, const char *restrict name)
{
	const layer_model_type_s *restrict t;
	if ((t = layer_model_find_type(m, name)) && t->create)
		return t->create(t, m);
	return NULL;
}

layer_model_item_s* layer_model_copyto_item(layer_model_item_s *restrict dst, const layer_model_item_s *restrict src)
{
	const layer_model_type_s *restrict t;
	if ((uintptr_t) dst != (uintptr_t) src && (t = dst->type) == src->type &&
		(dst->item_flag & layer_model_flag__write) && t->copyto)
		return t->copyto(t, dst, src);
	return NULL;
}

layer_model_item_s* layer_model_dumpit_item(const layer_model_s *restrict m, const layer_model_item_s *restrict item)
{
	const layer_model_type_s *restrict t;
	layer_model_item_s *restrict r;
	if ((t = item->type) && t->create && (r = t->create(t, m)))
	{
		if (layer_model_copyto_item(r, item))
			return r;
		refer_free(r);
	}
	return NULL;
}

void layer_model_iprint_item(struct layer_log_s *restrict log, const layer_model_item_s *restrict item)
{
	const layer_model_type_s *restrict t;
	if (log && item)
	{
		if ((t = item->type)->iprint)
			t->iprint(t, item, log);
		else
		{
			layer_model_item_iprint(item, log);
			mlog_printf(log->input, "\n");
		}
	}
}
