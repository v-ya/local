#include "layer.model.h"
#include <hashmap.h>
#include <exbuffer.h>

struct layer_model_any_s {
	exbuffer_t name_buffer;
	hashmap_t name_pool;
	const char *const *name_array;
	uintptr_t name_count;
	refer_string_t name;
};

static void layer_model_any_free_func(layer_model_any_s *restrict r)
{
	if (r->name) refer_free(r->name);
	exbuffer_uini(&r->name_buffer);
	hashmap_uini(&r->name_pool);
}

layer_model_any_s* layer_model_any_alloc(const char *restrict name)
{
	layer_model_any_s *restrict r;
	if (name && layer_model_type_name_enum(name) == layer_model_type__any &&
		(r = (layer_model_any_s *) refer_alloz(sizeof(layer_model_any_s))))
	{
		refer_set_free(r, (refer_free_f) layer_model_any_free_func);
		if (exbuffer_init(&r->name_buffer, 0) && hashmap_init(&r->name_pool) &&
			(r->name = refer_dump_string(name)))
			return r;
		refer_free(r);
	}
	return NULL;
}

refer_string_t layer_model_any_name(const layer_model_any_s *restrict any)
{
	return any->name;
}

layer_model_any_s* layer_model_any_insert(layer_model_any_s *restrict any, const char *restrict name)
{
	layer_model_type_t type;
	hashmap_vlist_t *restrict vl;
	if (name)
	{
		type = layer_model_type_name_enum(name);
		if (type != layer_model_type__unknow && type != layer_model_type__any &&
			!hashmap_find_name(&any->name_pool, name))
		{
			if ((vl = hashmap_set_name(&any->name_pool, name, NULL, NULL)))
			{
				if (exbuffer_append(&any->name_buffer, &vl->name, sizeof(vl->name)))
				{
					any->name_array = (const char *const *) any->name_buffer.data;
					any->name_count += 1;
					return any;
				}
				any->name_array = (const char *const *) any->name_buffer.data;
				hashmap_delete_name(&any->name_pool, name);
			}
		}
	}
	return NULL;
}

const char* layer_model_any_inherit(const layer_model_any_s *restrict any, const char *restrict name)
{
	const char *const *restrict p;
	const char *restrict parent;
	uintptr_t i, n;
	if (name)
	{
		p = any->name_array;
		n = any->name_count;
		for (i = 0; i < n; ++i)
		{
			if ((parent = layer_model_type_name_inherit(p[i], name)))
				return parent;
		}
	}
	return NULL;
}
