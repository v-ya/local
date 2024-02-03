#include "gvcx.model.h"
#include <exbuffer.h>

typedef struct gvcx_model_type_array_s gvcx_model_type_array_s;
typedef struct gvcx_model_item_array_s gvcx_model_item_array_s;
typedef struct gvcx_model_type_array_param_t gvcx_model_type_array_param_t;

struct gvcx_model_type_array_s {
	gvcx_model_type_s type;
	refer_string_t cname;
	const gvcx_model_any_s *cany;
};

struct gvcx_model_item_array_s {
	gvcx_model_item_s item;
	exbuffer_t item_buffer;
	const gvcx_model_s *model;
	gvcx_model_item_s **item_array;
	uintptr_t item_count;
};

struct gvcx_model_type_array_param_t {
	refer_string_t cname;
	const gvcx_model_any_s *cany;
};

static void gvcx_model_type_array_free_func(gvcx_model_type_array_s *restrict r)
{
	if (r->cany) refer_free(r->cany);
	if (r->cname) refer_free(r->cname);
	gvcx_model_type_free_func(&r->type);
}

static void gvcx_model_item_array_free_func(gvcx_model_item_array_s *restrict r)
{
	gvcx_model_set_a_clear(&r->item);
	if (r->model) refer_free(r->model);
	exbuffer_uini(&r->item_buffer);
	gvcx_model_item_free_func(&r->item);
}

static d_type_create(array, gvcx_model_type_array_s)
{
	gvcx_model_item_array_s *restrict r;
	if ((r = (gvcx_model_item_array_s *) gvcx_model_item_alloc(sizeof(gvcx_model_item_array_s), &t->type)))
	{
		refer_set_free(r, (refer_free_f) gvcx_model_item_array_free_func);
		if (exbuffer_init(&r->item_buffer, 0))
		{
			r->model = (const gvcx_model_s *) refer_save(m);
			return &r->item;
		}
		refer_free(r);
	}
	return NULL;
}
static d_type_copyto(array, gvcx_model_type_array_s, gvcx_model_item_array_s)
{
	gvcx_model_item_s **restrict p;
	gvcx_model_item_s *restrict item;
	uintptr_t i, n;
	p = src->item_array;
	n = src->item_count;
	for (i = 0; i < n; ++i)
	{
		if (!(item = gvcx_model_set_a_create(&dst->item, p[i]->type->name)))
			goto label_fail;
		if (!gvcx_model_copyto_item(item, p[i]))
			goto label_fail;
	}
	return &dst->item;
	label_fail:
	return NULL;
}
static d_type_initial(array, gvcx_model_type_array_param_t)
{
	refer_set_free(t, (refer_free_f) gvcx_model_type_array_free_func);
	((gvcx_model_type_array_s *) t)->cname = (refer_string_t) refer_save(pri->cname);
	((gvcx_model_type_array_s *) t)->cany = (const gvcx_model_any_s *) refer_save(pri->cany);
	t->create = d_type_function(array, create);
	t->copyto = d_type_function(array, copyto);
	return t;
}

const gvcx_model_type_s* gvcx_model_type_create__array(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname)
{
	gvcx_model_type_array_param_t param;
	param.cname = NULL;
	param.cany = NULL;
	if (!cname || (param.cname = gvcx_model_find_cname(m, cname, NULL, &param.cany)))
		return gvcx_model_type_alloc(name, gvcx_model_type__array, type_minor, sizeof(gvcx_model_type_array_s), d_type_function(array, initial), &param);
	return NULL;
}

// api

uintptr_t gvcx_model_get_a_count(const gvcx_model_item_s *restrict i)
{
	if (i->type_major == gvcx_model_type__array)
		return ((const gvcx_model_item_array_s *) i)->item_count;
	return 0;
}
gvcx_model_item_s* gvcx_model_get_a_index(const gvcx_model_item_s *restrict i, uintptr_t index)
{
	if (i->type_major == gvcx_model_type__array && index < ((const gvcx_model_item_array_s *) i)->item_count)
		return ((const gvcx_model_item_array_s *) i)->item_array[index];
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_a_clear(gvcx_model_item_s *restrict i)
{
	gvcx_model_item_s **restrict p;
	gvcx_model_item_s *restrict item;
	uintptr_t ii, n;
	if (i->type_major == gvcx_model_type__array && (i->item_flag & gvcx_model_flag__write))
	{
		p = ((gvcx_model_item_array_s *) i)->item_array;
		n = ((gvcx_model_item_array_s *) i)->item_count;
		for (ii = 0; ii < n; ++ii)
		{
			if ((item = p[ii]))
				refer_free(item);
		}
		exbuffer_clear(&((gvcx_model_item_array_s *) i)->item_buffer);
		((gvcx_model_item_array_s *) i)->item_array = NULL;
		((gvcx_model_item_array_s *) i)->item_count = 0;
		return i;
	}
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_a_append(gvcx_model_item_s *restrict i, gvcx_model_item_s *restrict item)
{
	const gvcx_model_type_array_s *restrict t;
	void *p;
	t = (const gvcx_model_type_array_s *) i->type;
	if (i->type_major == gvcx_model_type__array && (i->item_flag & gvcx_model_flag__write) &&
		item && gvcx_model_type_inherit_type(t->cname, t->cany, item->type) &&
		!(item->item_flag & gvcx_model_flag__child) &&
		(p = exbuffer_append(&((gvcx_model_item_array_s *) i)->item_buffer, (const void *) &item, sizeof(item))))
	{
		refer_save(item);
		((gvcx_model_item_array_s *) i)->item_array = (gvcx_model_item_s **) p;
		((gvcx_model_item_array_s *) i)->item_count += 1;
		item->item_flag |= gvcx_model_flag__child;
		return item;
	}
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_a_create(gvcx_model_item_s *restrict i, const char *restrict type)
{
	const gvcx_model_type_array_s *restrict t;
	gvcx_model_item_s *restrict item;
	void *p;
	t = (const gvcx_model_type_array_s *) i->type;
	if (i->type_major == gvcx_model_type__array && (i->item_flag & gvcx_model_flag__write) &&
		gvcx_model_type_inherit_name(t->cname, t->cany, type) &&
		(item = gvcx_model_create_item(((gvcx_model_item_array_s *) i)->model, type)))
	{
		if ((p = exbuffer_append(&((gvcx_model_item_array_s *) i)->item_buffer, (const void *) &item, sizeof(item))))
		{
			((gvcx_model_item_array_s *) i)->item_array = (gvcx_model_item_s **) p;
			((gvcx_model_item_array_s *) i)->item_count += 1;
			item->item_flag |= gvcx_model_flag__child;
			return item;
		}
		refer_free(item);
	}
	return NULL;
}
