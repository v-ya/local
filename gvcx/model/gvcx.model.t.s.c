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

static d_type_create(string, gvcx_model_type_s)
{
	gvcx_model_item_s *restrict r;
	if ((r = gvcx_model_item_alloc(sizeof(gvcx_model_item_string_s), t)))
		refer_set_free(r, (refer_free_f) gvcx_model_item_string_free_func);
	return r;
}
static d_type_copyto(string, gvcx_model_type_s, gvcx_model_item_string_s)
{
	if ((dst->value)) refer_free(dst->value);
	dst->value = (refer_nstring_t) refer_save(src->value);
	return &dst->item;
}
static d_type_iprint(string, gvcx_model_type_s, gvcx_model_item_string_s)
{
	if (gvcx_model_item_iprint(&item->item, log))
	{
		if (item->value)
			mlog_printf(log->input, " = %s\n", item->value->string);
		else mlog_printf(log->input, " -\n");
	}
}
static d_type_initial(string, void)
{
	t->create = d_type_function(string, create);
	t->copyto = d_type_function(string, copyto);
	t->iprint = d_type_function(string, iprint);
	return t;
}

const gvcx_model_type_s* gvcx_model_type_create__string(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor)
{
	return gvcx_model_type_alloc(name, gvcx_model_type__string, type_minor, sizeof(gvcx_model_type_s), d_type_function(string, initial), NULL);
}

// api

refer_nstring_t gvcx_model_get_s(const gvcx_model_item_s *restrict i)
{
	if (i->type_major == gvcx_model_type__string)
		return ((const gvcx_model_item_string_s *) i)->value;
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_s(gvcx_model_item_s *restrict i, refer_nstring_t v)
{
	if (i->type_major == gvcx_model_type__string && (i->item_flag & gvcx_model_flag__write))
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
