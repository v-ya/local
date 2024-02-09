#include "layer.model.h"
#include <inttypes.h>

typedef struct layer_model_type_enum_s layer_model_type_enum_s;
typedef struct layer_model_item_enum_s layer_model_item_enum_s;

struct layer_model_type_enum_s {
	layer_model_type_s type;
	const layer_model_enum_s *e;
};

struct layer_model_item_enum_s {
	layer_model_item_s item;
	uint64_t value;
};

static void layer_model_type_enum_free_func(layer_model_type_enum_s *restrict r)
{
	if (r->e) refer_free(r->e);
	layer_model_type_free_func(&r->type);
}

static d_type_create(enum, layer_model_type_enum_s)
{
	layer_model_item_s *restrict r;
	if ((r = layer_model_item_alloc(sizeof(layer_model_item_enum_s), &t->type)))
		((layer_model_item_enum_s *) r)->value = layer_model_enum_dv(t->e);
	return r;
}
static d_type_copyto(enum, layer_model_type_enum_s, layer_model_item_enum_s)
{
	dst->value = src->value;
	return &dst->item;
}
static d_type_iprint(enum, layer_model_type_enum_s, layer_model_item_enum_s)
{
	if (layer_model_item_iprint(&item->item, log))
		mlog_printf(log->input, " = %" PRIi64 " (%s)\n", item->value, layer_model_enum_name(t->e, item->value));
}
static d_type_initial(enum, layer_model_enum_s)
{
	refer_set_free(t, (refer_free_f) layer_model_type_enum_free_func);
	((layer_model_type_enum_s *) t)->e = (const layer_model_enum_s *) refer_save(pri);
	t->create = d_type_function(enum, create);
	t->copyto = d_type_function(enum, copyto);
	t->iprint = d_type_function(enum, iprint);
	return t;
}

const layer_model_type_s* layer_model_type_create__enum(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict ename)
{
	const layer_model_enum_s *restrict e;
	if (ename && (e = layer_model_find_enum(m, ename)))
		return layer_model_type_alloc(name, layer_model_type__enum, type_minor, sizeof(layer_model_type_enum_s), d_type_function(enum, initial), e);
	return NULL;
}

// api

const layer_model_item_s* layer_model_get_e(const layer_model_item_s *restrict i, uint64_t *restrict value, const char *restrict *restrict name)
{
	if (i->type_major == layer_model_type__enum)
	{
		if (value) *value = ((const layer_model_item_enum_s *) i)->value;
		if (name) *name = layer_model_enum_name(((const layer_model_type_enum_s *) i->type)->e, ((const layer_model_item_enum_s *) i)->value);
		return i;
	}
	return NULL;
}
uint64_t layer_model_get_e_value(const layer_model_item_s *restrict i)
{
	if (i->type_major == layer_model_type__enum)
		return ((const layer_model_item_enum_s *) i)->value;
	return 0;
}
const char* layer_model_get_e_name(const layer_model_item_s *restrict i)
{
	if (i->type_major == layer_model_type__enum)
		return layer_model_enum_name(((const layer_model_type_enum_s *) i->type)->e, ((const layer_model_item_enum_s *) i)->value);
	return NULL;
}
layer_model_item_s* layer_model_set_e_value(layer_model_item_s *restrict i, uint64_t value)
{
	if (i->type_major == layer_model_type__enum && (i->item_flag & layer_model_flag__write) &&
		layer_model_enum_name(((const layer_model_type_enum_s *) i->type)->e, value))
	{
		((layer_model_item_enum_s *) i)->value = value;
		return i;
	}
	return NULL;
}
layer_model_item_s* layer_model_set_e_name(layer_model_item_s *restrict i, const char *restrict name)
{
	uint64_t value;
	if (i->type_major == layer_model_type__enum && (i->item_flag & layer_model_flag__write) &&
		name && layer_model_enum_value(((const layer_model_type_enum_s *) i->type)->e, name, &value))
	{
		((layer_model_item_enum_s *) i)->value = value;
		return i;
	}
	return NULL;
}
