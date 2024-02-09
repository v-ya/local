#include "layer.model.h"

typedef struct layer_model_item_string_s layer_model_item_string_s;

struct layer_model_item_string_s {
	layer_model_item_s item;
	refer_nstring_t value;
};

static void layer_model_item_string_free_func(layer_model_item_string_s *restrict r)
{
	if (r->value) refer_free(r->value);
	layer_model_item_free_func(&r->item);
}

static d_type_create(string, layer_model_type_s)
{
	layer_model_item_s *restrict r;
	if ((r = layer_model_item_alloc(sizeof(layer_model_item_string_s), t)))
		refer_set_free(r, (refer_free_f) layer_model_item_string_free_func);
	return r;
}
static d_type_copyto(string, layer_model_type_s, layer_model_item_string_s)
{
	if ((dst->value)) refer_free(dst->value);
	dst->value = (refer_nstring_t) refer_save(src->value);
	return &dst->item;
}
static d_type_iprint(string, layer_model_type_s, layer_model_item_string_s)
{
	if (layer_model_item_iprint(&item->item, log))
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

const layer_model_type_s* layer_model_type_create__string(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor)
{
	return layer_model_type_alloc(name, layer_model_type__string, type_minor, sizeof(layer_model_type_s), d_type_function(string, initial), NULL);
}

// api

refer_nstring_t layer_model_get_s(const layer_model_item_s *restrict i)
{
	if (i->type_major == layer_model_type__string)
		return ((const layer_model_item_string_s *) i)->value;
	return NULL;
}
layer_model_item_s* layer_model_set_s(layer_model_item_s *restrict i, refer_nstring_t v)
{
	if (i->type_major == layer_model_type__string && (i->item_flag & layer_model_flag__write))
	{
		refer_save(v);
		if (((layer_model_item_string_s *) i)->value)
			refer_free(((layer_model_item_string_s *) i)->value);
		((layer_model_item_string_s *) i)->value = v;
		return i;
	}
	return NULL;
}
layer_model_item_s* layer_model_set_s_str(layer_model_item_s *restrict i, const char *restrict v)
{
	refer_nstring_t nstr;
	if ((nstr = v?refer_dump_nstring(v):NULL) || !v)
	{
		i = layer_model_set_s(i, nstr);
		if (nstr) refer_free(nstr);
		return i;
	}
	return NULL;
}
layer_model_item_s* layer_model_set_s_str_len(layer_model_item_s *restrict i, const char *restrict v, uintptr_t n)
{
	refer_nstring_t nstr;
	if ((nstr = refer_dump_nstring_with_length(v, n)))
	{
		i = layer_model_set_s(i, nstr);
		refer_free(nstr);
		return i;
	}
	return NULL;
}
