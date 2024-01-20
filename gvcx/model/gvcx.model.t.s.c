#include "gvcx.model.h"

typedef struct gvcx_model_item_string_s gvcx_model_item_string_s;

struct gvcx_model_item_string_s {
	gvcx_model_item_s item;
	refer_nstring_t value;
};

static void gvcx_model_item_string_free_func(gvcx_model_item_string_s *restrict r)
{
	if (r->value) refer_free(r->value);
	gvcx_model_item_free_func(&r->item);
}

static gvcx_model_item_s* gvcx_model_type_create(const gvcx_model_type_s *restrict t, const gvcx_model_s *restrict m, gvcx_model_type_t type, gvcx_model_flag_t flag, const char *restrict tname)
{
	gvcx_model_item_s *restrict r;
	if ((r = gvcx_model_item_alloc(sizeof(gvcx_model_item_string_s), type, flag, t->tname)))
		refer_set_free(r, (refer_free_f) gvcx_model_item_string_free_func);
	return r;
}
static gvcx_model_item_s* gvcx_model_type_copyto(const gvcx_model_type_s *restrict t, gvcx_model_item_string_s *restrict dst, const gvcx_model_item_string_s *restrict src)
{
	if ((dst->value)) refer_free(dst->value);
	dst->value = (refer_nstring_t) refer_save(src->value);
	return &dst->item;
}
static gvcx_model_type_s* gvcx_model_type_initial(gvcx_model_type_s *restrict t, const void *restrict pri)
{
	t->create = (gvcx_model_type_create_f) gvcx_model_type_create;
	t->copyto = (gvcx_model_type_copyto_f) gvcx_model_type_copyto;
	return t;
}

const gvcx_model_type_s* gvcx_model_type_create__s(void)
{
	return gvcx_model_type_alloc(gvcx_model_stype__string, sizeof(gvcx_model_type_s), gvcx_model_type_initial, NULL);
}

// api

refer_nstring_t gvcx_model_get_s(const gvcx_model_item_s *restrict i)
{
	if (i->type == gvcx_model_type__s)
		return ((const gvcx_model_item_string_s *) i)->value;
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_s(gvcx_model_item_s *restrict i, refer_nstring_t v)
{
	if (i->type == gvcx_model_type__s && (i->flag & gvcx_model_flag__write))
	{
		refer_save(v);
		if (((gvcx_model_item_string_s *) i)->value)
			refer_free(((gvcx_model_item_string_s *) i)->value);
		((gvcx_model_item_string_s *) i)->value = v;
		return i;
	}
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_s_str(gvcx_model_item_s *restrict i, const char *restrict v)
{
	refer_nstring_t nstr;
	if ((nstr = v?refer_dump_nstring(v):NULL) || !v)
	{
		i = gvcx_model_set_s(i, nstr);
		if (nstr) refer_free(nstr);
		return i;
	}
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_s_str_len(gvcx_model_item_s *restrict i, const char *restrict v, uintptr_t n)
{
	refer_nstring_t nstr;
	if ((nstr = refer_dump_nstring_with_length(v, n)))
	{
		i = gvcx_model_set_s(i, nstr);
		refer_free(nstr);
		return i;
	}
	return NULL;
}
