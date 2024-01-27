#include "gvcx.model.h"

// pri

const gvcx_model_type_s* gvcx_model_find_type(const gvcx_model_s *restrict m, const char *restrict name)
{
	return (const gvcx_model_type_s *) vattr_get_first(m->pool_type, name);
}
const gvcx_model_any_s* gvcx_model_find_any(const gvcx_model_s *restrict m, const char *restrict name)
{
	return (const gvcx_model_any_s *) vattr_get_first(m->pool_any, name);
}
const gvcx_model_enum_s* gvcx_model_find_enum(const gvcx_model_s *restrict m, const char *restrict name)
{
	return (const gvcx_model_enum_s *) vattr_get_first(m->pool_enum, name);
}
const gvcx_model_object_s* gvcx_model_find_object(const gvcx_model_s *restrict m, const char *restrict name)
{
	return (const gvcx_model_object_s *) vattr_get_first(m->pool_object, name);
}

gvcx_model_s* gvcx_model_add_type(gvcx_model_s *restrict m, const gvcx_model_type_s *restrict type)
{
	if (type && !vattr_get_vslot(m->pool_type, type->name) &&
		vattr_insert_tail(m->pool_type, type->name, type))
		return m;
	return NULL;
}
gvcx_model_s* gvcx_model_add_any(gvcx_model_s *restrict m, const gvcx_model_any_s *restrict any)
{
	refer_string_t name;
	if (any && (name = gvcx_model_any_name(any)) &&
		!vattr_get_vslot(m->pool_any, name) &&
		vattr_insert_tail(m->pool_any, name, any))
		return m;
	return NULL;
}
gvcx_model_s* gvcx_model_add_enum(gvcx_model_s *restrict m, const char *restrict name, const gvcx_model_enum_s *restrict e)
{
	if (name && e && !vattr_get_vslot(m->pool_enum, name) &&
		vattr_insert_tail(m->pool_enum, name, e))
		return m;
	return NULL;
}
gvcx_model_s* gvcx_model_add_object(gvcx_model_s *restrict m, const char *restrict name, const gvcx_model_object_s *restrict object)
{
	if (name && object && !vattr_get_vslot(m->pool_object, name) &&
		vattr_insert_tail(m->pool_object, name, object))
		return m;
	return NULL;
}

// api

static void gvcx_model_free_func(refer_t *restrict r)
{
	uintptr_t i, n;
	for (i = 0, n = sizeof(gvcx_model_s) / sizeof(refer_t); i < n; ++i)
	{
		if (r[i]) refer_free(r[i]);
	}
}

gvcx_model_s* gvcx_model_alloc(void)
{
	gvcx_model_s *restrict r;
	if ((r = (gvcx_model_s *) refer_alloz(sizeof(gvcx_model_s))))
	{
		refer_set_free(r, (refer_free_f) gvcx_model_free_func);
		if (!(r->pool_type = vattr_alloc())) goto label_fail;
		if (!(r->pool_any = vattr_alloc())) goto label_fail;
		if (!(r->pool_enum = vattr_alloc())) goto label_fail;
		if (!(r->pool_object = vattr_alloc())) goto label_fail;
		#define d_type(_t, ...)  if (!gvcx_model_create_type__##_t(r, gvcx_model_stype__##_t, 0, ##__VA_ARGS__)) goto label_fail
		d_type(uint);
		d_type(int);
		d_type(float);
		d_type(boolean);
		d_type(string);
		d_type(data);
		d_type(array, NULL);
		d_type(object, NULL);
		#undef d_type
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

gvcx_model_item_s* gvcx_model_create_item(const gvcx_model_s *restrict m, const char *restrict name)
{
	const gvcx_model_type_s *restrict t;
	if ((t = gvcx_model_find_type(m, name)) && t->create)
		return t->create(t);
	return NULL;
}

gvcx_model_item_s* gvcx_model_copyto_item(const gvcx_model_s *restrict m, gvcx_model_item_s *restrict dst, const gvcx_model_item_s *restrict src)
{
	const gvcx_model_type_s *restrict t;
	if ((uintptr_t) dst != (uintptr_t) src && (t = dst->type) == src->type &&
		(dst->item_flag & gvcx_model_flag__write) && t->copyto)
		return t->copyto(t, dst, src);
	return NULL;
}
