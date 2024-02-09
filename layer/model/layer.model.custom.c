#include "layer.model.h"

// type

#define d_create_type(_t, ...)  \
	{\
		const layer_model_type_s *restrict t;\
		if ((t = layer_model_type_create__##_t(m, name, type_minor, ##__VA_ARGS__)))\
		{\
			m = layer_model_add_type(m, t);\
			refer_free(t);\
			return m;\
		}\
		return NULL;\
	}

layer_model_s* layer_model_create_type__null(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(null)
layer_model_s* layer_model_create_type__uint(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(uint)
layer_model_s* layer_model_create_type__int(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(int)
layer_model_s* layer_model_create_type__float(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(float)
layer_model_s* layer_model_create_type__boolean(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(boolean)
layer_model_s* layer_model_create_type__string(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(string)
layer_model_s* layer_model_create_type__data(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(data)
layer_model_s* layer_model_create_type__enum(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict ename) d_create_type(enum, ename)
layer_model_s* layer_model_create_type__array(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname) d_create_type(array, cname)
layer_model_s* layer_model_create_type__object(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname, const char *restrict oname) d_create_type(object, cname, oname)

#undef d_create_type

// any

layer_model_s* layer_model_create_any(layer_model_s *restrict m, const char *restrict name, const char *const restrict *restrict list)
{
	layer_model_any_s *restrict a;
	if (name && list && *list)
	{
		if ((a = layer_model_any_alloc(name)))
		{
			while (*list)
			{
				if (!layer_model_any_insert(a, *list))
					goto label_fail;
				list += 1;
			}
			m = layer_model_add_any(m, a);
			refer_free(a);
			return m;
			label_fail:
			refer_free(a);
		}
	}
	return NULL;
}

// enum

layer_model_s* layer_model_create_enum(layer_model_s *restrict m, const char *restrict name, const layer_model_custom_enum_t *restrict list, uint64_t dv)
{
	layer_model_enum_s *restrict e;
	if (name && list && list->name)
	{
		if ((e = layer_model_enum_alloc(dv)))
		{
			while (list->name)
			{
				if (!layer_model_enum_insert(e, list->name, list->value))
					goto label_fail;
				list += 1;
			}
			if (layer_model_enum_name(e, layer_model_enum_dv(e)))
			{
				m = layer_model_add_enum(m, name, e);
				refer_free(e);
				return m;
			}
			label_fail:
			refer_free(e);
		}
	}
	return NULL;
}

// object

static layer_model_object_s* layer_model_create_object_insert(layer_model_object_s *restrict o, const layer_model_s *restrict m, const layer_model_custom_object_t *restrict item)
{
	refer_string_t name;
	const layer_model_any_s *restrict any;
	layer_model_item_s *restrict dv;
	name = NULL;
	any = NULL;
	dv = NULL;
	if (item->name && !(name = layer_model_find_cname(m, item->name, NULL, &any)))
		goto label_fail;
	if (item->func && !(dv = item->func(m)))
		goto label_fail;
	if (!layer_model_object_insert(o, item->key, name, any, dv))
		goto label_fail;
	if (dv) refer_free(dv);
	return o;
	label_fail:
	if (dv) refer_free(dv);
	return NULL;
}
layer_model_s* layer_model_create_object(layer_model_s *restrict m, const char *restrict name, const layer_model_custom_object_t *restrict list)
{
	layer_model_object_s *restrict o;
	if (name && list && list->key)
	{
		if ((o = layer_model_object_alloc()))
		{
			while (list->key)
			{
				if (!layer_model_create_object_insert(o, m, list))
					goto label_fail;
				list += 1;
			}
			m = layer_model_add_object(m, name, o);
			refer_free(o);
			return m;
			label_fail:
			refer_free(o);
		}
	}
	return NULL;
}
