#include "gvcx.model.h"

// type

#define d_create_type(_t, ...)  \
	{\
		const gvcx_model_type_s *restrict t;\
		if ((t = gvcx_model_type_create__##_t(m, name, type_minor, ##__VA_ARGS__)))\
		{\
			m = gvcx_model_add_type(m, t);\
			refer_free(t);\
			return m;\
		}\
		return NULL;\
	}

gvcx_model_s* gvcx_model_create_type__uint(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(uint)
gvcx_model_s* gvcx_model_create_type__int(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(int)
gvcx_model_s* gvcx_model_create_type__float(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(float)
gvcx_model_s* gvcx_model_create_type__boolean(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(boolean)
gvcx_model_s* gvcx_model_create_type__string(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(string)
gvcx_model_s* gvcx_model_create_type__data(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor) d_create_type(data)
gvcx_model_s* gvcx_model_create_type__enum(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict ename) d_create_type(enum, ename)
gvcx_model_s* gvcx_model_create_type__array(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname) d_create_type(array, cname)
gvcx_model_s* gvcx_model_create_type__object(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname) d_create_type(object, cname)

#undef d_create_type

// any

gvcx_model_s* gvcx_model_create_any(gvcx_model_s *restrict m, const char *restrict name, const char *const restrict *restrict list)
{
	gvcx_model_any_s *restrict a;
	if (name && list && *list)
	{
		if ((a = gvcx_model_any_alloc(name)))
		{
			while (*list)
			{
				if (!gvcx_model_find_type(m, *list))
					goto label_fail;
				if (!gvcx_model_any_insert(a, *list))
					goto label_fail;
				list += 1;
			}
			m = gvcx_model_add_any(m, a);
			refer_free(a);
			return m;
			label_fail:
			refer_free(a);
		}
	}
	return NULL;
}

// enum

gvcx_model_s* gvcx_model_create_enum(gvcx_model_s *restrict m, const char *restrict name, const gvcx_model_custom_enum_t *restrict list, uint64_t dv)
{
	gvcx_model_enum_s *restrict e;
	if (name && list && list->name)
	{
		if ((e = gvcx_model_enum_alloc(dv)))
		{
			while (list->name)
			{
				if (!gvcx_model_enum_insert(e, list->name, list->value))
					goto label_fail;
				list += 1;
			}
			if (gvcx_model_enum_name(e, gvcx_model_enum_dv(e)))
			{
				m = gvcx_model_add_enum(m, name, e);
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

static refer_string_t gvcx_model_create_object_name(const gvcx_model_s *restrict m, const char *restrict name)
{
	const gvcx_model_type_s *restrict type;
	const gvcx_model_any_s *restrict any;
	if ((type = gvcx_model_find_type(m, name)))
		return type->name;
	else if ((any = gvcx_model_find_any(m, name)))
		return gvcx_model_any_name(any);
	return NULL;
}
static gvcx_model_object_s* gvcx_model_create_object_insert(gvcx_model_object_s *restrict o, const gvcx_model_s *restrict m, const gvcx_model_custom_object_t *restrict item)
{
	gvcx_model_item_s *restrict dv;
	refer_string_t name;
	name = NULL;
	dv = NULL;
	if (item->name && !(name = gvcx_model_create_object_name(m, item->name)))
		goto label_fail;
	if (item->func && !(dv = item->func(m)))
		goto label_fail;
	if (!gvcx_model_object_insert(o, item->key, name, dv))
		goto label_fail;
	label_clear:
	if (dv) refer_free(dv);
	if (name) refer_free(name);
	return o;
	label_fail:
	o = NULL;
	goto label_clear;
}
gvcx_model_s* gvcx_model_create_object(gvcx_model_s *restrict m, const char *restrict name, const gvcx_model_custom_object_t *restrict list)
{
	gvcx_model_object_s *restrict o;
	if (name && list && list->key)
	{
		if ((o = gvcx_model_object_alloc()))
		{
			while (list->key)
			{
				if (!gvcx_model_create_object_insert(o, m, list))
					goto label_fail;
				list += 1;
			}
			m = gvcx_model_add_object(m, name, o);
			refer_free(o);
			return m;
			label_fail:
			refer_free(o);
		}
	}
	return NULL;
}
