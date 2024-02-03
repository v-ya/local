#include "gvcx.model.h"

typedef struct gvcx_model_type_enum_s gvcx_model_type_enum_s;
typedef struct gvcx_model_item_enum_s gvcx_model_item_enum_s;

struct gvcx_model_type_enum_s {
	gvcx_model_type_s type;
	const gvcx_model_enum_s *e;
};

struct gvcx_model_item_enum_s {
	gvcx_model_item_s item;
	uint64_t value;
};

static void gvcx_model_type_enum_free_func(gvcx_model_type_enum_s *restrict r)
{
	if (r->e) refer_free(r->e);
	gvcx_model_type_free_func(&r->type);
}

static d_type_create(enum, gvcx_model_type_enum_s)
{
	gvcx_model_item_s *restrict r;
	if ((r = gvcx_model_item_alloc(sizeof(gvcx_model_item_enum_s), &t->type)))
		((gvcx_model_item_enum_s *) r)->value = gvcx_model_enum_dv(t->e);
	return r;
}
static d_type_copyto(enum, gvcx_model_type_enum_s, gvcx_model_item_enum_s)
{
	dst->value = src->value;
	return &dst->item;
}
static d_type_initial(enum, gvcx_model_enum_s)
{
	refer_set_free(t, (refer_free_f) gvcx_model_type_enum_free_func);
	((gvcx_model_type_enum_s *) t)->e = (const gvcx_model_enum_s *) refer_save(pri);
	t->create = d_type_function(enum, create);
	t->copyto = d_type_function(enum, copyto);
	return t;
}

const gvcx_model_type_s* gvcx_model_type_create__enum(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict ename)
{
	const gvcx_model_enum_s *restrict e;
	if (ename && (e = gvcx_model_find_enum(m, ename)))
		return gvcx_model_type_alloc(name, gvcx_model_type__enum, type_minor, sizeof(gvcx_model_type_enum_s), d_type_function(enum, initial), e);
	return NULL;
}

// api

const gvcx_model_item_s* gvcx_model_get_e(const gvcx_model_item_s *restrict i, uint64_t *restrict value, const char *restrict *restrict name)
{
	if (i->type_major == gvcx_model_type__enum)
	{
		if (value) *value = ((const gvcx_model_item_enum_s *) i)->value;
		if (name) *name = gvcx_model_enum_name(((const gvcx_model_type_enum_s *) i->type)->e, ((const gvcx_model_item_enum_s *) i)->value);
		return i;
	}
	return NULL;
}
uint64_t gvcx_model_get_e_value(const gvcx_model_item_s *restrict i)
{
	if (i->type_major == gvcx_model_type__enum)
		return ((const gvcx_model_item_enum_s *) i)->value;
	return 0;
}
const char* gvcx_model_get_e_name(const gvcx_model_item_s *restrict i)
{
	if (i->type_major == gvcx_model_type__enum)
		return gvcx_model_enum_name(((const gvcx_model_type_enum_s *) i->type)->e, ((const gvcx_model_item_enum_s *) i)->value);
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_e_value(gvcx_model_item_s *restrict i, uint64_t value)
{
	if (i->type_major == gvcx_model_type__enum && (i->item_flag & gvcx_model_flag__write) &&
		gvcx_model_enum_name(((const gvcx_model_type_enum_s *) i->type)->e, value))
	{
		((gvcx_model_item_enum_s *) i)->value = value;
		return i;
	}
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_e_name(gvcx_model_item_s *restrict i, const char *restrict name)
{
	uint64_t value;
	if (i->type_major == gvcx_model_type__enum && (i->item_flag & gvcx_model_flag__write) &&
		name && gvcx_model_enum_value(((const gvcx_model_type_enum_s *) i->type)->e, name, &value))
	{
		((gvcx_model_item_enum_s *) i)->value = value;
		return i;
	}
	return NULL;
}
