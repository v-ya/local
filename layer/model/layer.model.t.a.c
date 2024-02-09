#include "layer.model.h"
#include <exbuffer.h>

typedef struct layer_model_type_array_s layer_model_type_array_s;
typedef struct layer_model_item_array_s layer_model_item_array_s;
typedef struct layer_model_type_array_param_t layer_model_type_array_param_t;

struct layer_model_type_array_s {
	layer_model_type_s type;
	refer_string_t cname;
	const layer_model_any_s *cany;
};

struct layer_model_item_array_s {
	layer_model_item_s item;
	exbuffer_t item_buffer;
	const layer_model_s *model;
	layer_model_item_s **item_array;
	uintptr_t item_count;
};

struct layer_model_type_array_param_t {
	refer_string_t cname;
	const layer_model_any_s *cany;
};

static void layer_model_type_array_free_func(layer_model_type_array_s *restrict r)
{
	if (r->cany) refer_free(r->cany);
	if (r->cname) refer_free(r->cname);
	layer_model_type_free_func(&r->type);
}

static void layer_model_item_array_free_func(layer_model_item_array_s *restrict r)
{
	layer_model_set_a_clear(&r->item);
	if (r->model) refer_free(r->model);
	exbuffer_uini(&r->item_buffer);
	layer_model_item_free_func(&r->item);
}

static d_type_create(array, layer_model_type_array_s)
{
	layer_model_item_array_s *restrict r;
	if ((r = (layer_model_item_array_s *) layer_model_item_alloc(sizeof(layer_model_item_array_s), &t->type)))
	{
		refer_set_free(r, (refer_free_f) layer_model_item_array_free_func);
		if (exbuffer_init(&r->item_buffer, 0))
		{
			r->model = (const layer_model_s *) refer_save(m);
			return &r->item;
		}
		refer_free(r);
	}
	return NULL;
}
static d_type_copyto(array, layer_model_type_array_s, layer_model_item_array_s)
{
	layer_model_item_s **restrict p;
	layer_model_item_s *restrict item;
	uintptr_t i, n;
	p = src->item_array;
	n = src->item_count;
	for (i = 0; i < n; ++i)
	{
		if (!(item = layer_model_set_a_create(&dst->item, p[i]->type->name)))
			goto label_fail;
		if (!layer_model_copyto_item(item, p[i]))
			goto label_fail;
	}
	return &dst->item;
	label_fail:
	return NULL;
}
static d_type_iprint(array, layer_model_type_array_s, layer_model_item_array_s)
{
	layer_model_item_s **restrict p;
	uintptr_t i, n;
	if (layer_model_item_iprint(&item->item, log))
	{
		mlog_printf(log->input, "@(%s) count = %zu\n", t->cname?t->cname:"-", item->item_count);
		if (layer_log_push(log, 1))
		{
			p = item->item_array;
			n = item->item_count;
			for (i = 0; i < n; ++i)
			{
				mlog_printf(log->input, "[%zu]: ", i);
				layer_model_iprint_item(log, p[i]);
			}
			layer_log_pop(log, 1);
		}
	}
}
static d_type_initial(array, layer_model_type_array_param_t)
{
	refer_set_free(t, (refer_free_f) layer_model_type_array_free_func);
	((layer_model_type_array_s *) t)->cname = (refer_string_t) refer_save(pri->cname);
	((layer_model_type_array_s *) t)->cany = (const layer_model_any_s *) refer_save(pri->cany);
	t->create = d_type_function(array, create);
	t->copyto = d_type_function(array, copyto);
	t->iprint = d_type_function(array, iprint);
	return t;
}

const layer_model_type_s* layer_model_type_create__array(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname)
{
	layer_model_type_array_param_t param;
	param.cname = NULL;
	param.cany = NULL;
	if (!cname || (param.cname = layer_model_find_cname(m, cname, NULL, &param.cany)))
		return layer_model_type_alloc(name, layer_model_type__array, type_minor, sizeof(layer_model_type_array_s), d_type_function(array, initial), &param);
	return NULL;
}

// api

uintptr_t layer_model_get_a_count(const layer_model_item_s *restrict i)
{
	if (i->type_major == layer_model_type__array)
		return ((const layer_model_item_array_s *) i)->item_count;
	return 0;
}
layer_model_item_s* layer_model_get_a_index(const layer_model_item_s *restrict i, uintptr_t index)
{
	if (i->type_major == layer_model_type__array && index < ((const layer_model_item_array_s *) i)->item_count)
		return ((const layer_model_item_array_s *) i)->item_array[index];
	return NULL;
}
layer_model_item_s* layer_model_set_a_clear(layer_model_item_s *restrict i)
{
	layer_model_item_s **restrict p;
	layer_model_item_s *restrict item;
	uintptr_t ii, n;
	if (i->type_major == layer_model_type__array && (i->item_flag & layer_model_flag__write))
	{
		p = ((layer_model_item_array_s *) i)->item_array;
		n = ((layer_model_item_array_s *) i)->item_count;
		for (ii = 0; ii < n; ++ii)
		{
			if ((item = p[ii]))
				refer_free(item);
		}
		exbuffer_clear(&((layer_model_item_array_s *) i)->item_buffer);
		((layer_model_item_array_s *) i)->item_array = NULL;
		((layer_model_item_array_s *) i)->item_count = 0;
		return i;
	}
	return NULL;
}
layer_model_item_s* layer_model_set_a_append(layer_model_item_s *restrict i, layer_model_item_s *restrict item)
{
	const layer_model_type_array_s *restrict t;
	void *p;
	t = (const layer_model_type_array_s *) i->type;
	if (i->type_major == layer_model_type__array && (i->item_flag & layer_model_flag__write) &&
		item && layer_model_type_inherit_type(t->cname, t->cany, item->type) &&
		!(item->item_flag & layer_model_flag__child) &&
		(p = exbuffer_append(&((layer_model_item_array_s *) i)->item_buffer, (const void *) &item, sizeof(item))))
	{
		refer_save(item);
		((layer_model_item_array_s *) i)->item_array = (layer_model_item_s **) p;
		((layer_model_item_array_s *) i)->item_count += 1;
		item->item_flag |= layer_model_flag__child;
		return item;
	}
	return NULL;
}
layer_model_item_s* layer_model_set_a_create(layer_model_item_s *restrict i, const char *restrict type)
{
	const layer_model_type_array_s *restrict t;
	layer_model_item_s *restrict item;
	void *p;
	t = (const layer_model_type_array_s *) i->type;
	if (i->type_major == layer_model_type__array && (i->item_flag & layer_model_flag__write) &&
		layer_model_type_inherit_name(t->cname, t->cany, type) &&
		(item = layer_model_create_item(((layer_model_item_array_s *) i)->model, type)))
	{
		if ((p = exbuffer_append(&((layer_model_item_array_s *) i)->item_buffer, (const void *) &item, sizeof(item))))
		{
			((layer_model_item_array_s *) i)->item_array = (layer_model_item_s **) p;
			((layer_model_item_array_s *) i)->item_count += 1;
			item->item_flag |= layer_model_flag__child;
			return item;
		}
		refer_free(item);
	}
	return NULL;
}
