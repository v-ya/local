#include "layer.model.h"

typedef struct layer_model_type_object_s layer_model_type_object_s;
typedef struct layer_model_item_object_s layer_model_item_object_s;
typedef struct layer_model_type_object_param_t layer_model_type_object_param_t;

struct layer_model_type_object_s {
	layer_model_type_s type;
	refer_string_t cname;
	const layer_model_any_s *cany;
	const layer_model_object_s *o;
};

struct layer_model_item_object_s {
	layer_model_item_s item;
	const layer_model_s *model;
	vattr_s *object;
};

struct layer_model_type_object_param_t {
	refer_string_t cname;
	const layer_model_any_s *cany;
	const layer_model_object_s *o;
};

static const char* layer_model_type_object_inherit_name(const layer_model_type_object_s *restrict o, const char *restrict key, const char *restrict name)
{
	refer_string_t cname;
	const layer_model_any_s *restrict cany;
	if (!o->o || !layer_model_object_find(o->o, key, &cname, &cany, NULL))
		cname = o->cname, cany = o->cany;
	return layer_model_type_inherit_name(cname, cany, name);
}

static const layer_model_type_s* layer_model_type_object_inherit_type(const layer_model_type_object_s *restrict o, const char *restrict key, const layer_model_type_s *restrict type)
{
	refer_string_t cname;
	const layer_model_any_s *restrict cany;
	if (!o->o || !layer_model_object_find(o->o, key, &cname, &cany, NULL))
		cname = o->cname, cany = o->cany;
	return layer_model_type_inherit_type(cname, cany, type);
}

static void layer_model_type_object_iprint_child(const layer_model_item_s *restrict item, layer_log_s *restrict log, const char *restrict key, int ctype)
{
	mlog_printf(log->input, "\"%s\"%c ", key, ctype);
	if (item) layer_model_iprint_item(log, item);
	else mlog_printf(log->input, "-\n");
}

static void layer_model_type_object_free_func(layer_model_type_object_s *restrict r)
{
	if (r->o) refer_free(r->o);
	if (r->cany) refer_free(r->cany);
	if (r->cname) refer_free(r->cname);
	layer_model_type_free_func(&r->type);
}

static void layer_model_item_object_free_func(layer_model_item_object_s *restrict r)
{
	if (r->object) refer_free(r->object);
	if (r->model) refer_free(r->model);
	layer_model_item_free_func(&r->item);
}

static d_type_create(object, layer_model_type_object_s)
{
	layer_model_item_object_s *restrict r;
	if ((r = (layer_model_item_object_s *) layer_model_item_alloc(sizeof(layer_model_item_object_s), &t->type)))
	{
		refer_set_free(r, (refer_free_f) layer_model_item_object_free_func);
		r->model = (const layer_model_s *) refer_save(m);
		if ((r->object = vattr_alloc()))
			return &r->item;
		refer_free(r);
	}
	return NULL;
}
static d_type_copyto(object, layer_model_type_object_s, layer_model_item_object_s)
{
	const layer_model_item_s *restrict si;
	layer_model_item_s *restrict di;
	vattr_vlist_t *restrict vl;
	for (vl = src->object->vattr; vl; vl = vl->vattr_next)
	{
		si = (const layer_model_item_s *) vl->value;
		if (!(di = layer_model_set_o_create(&dst->item, vl->vslot->key, si->type->name)))
			goto label_fail;
		if (!layer_model_copyto_item(di, si))
			goto label_fail;
	}
	return &dst->item;
	label_fail:
	return NULL;
}
static d_type_iprint(object, layer_model_type_object_s, layer_model_item_object_s)
{
	const vattr_s *restrict ov;
	vattr_vlist_t *restrict vl;
	const char *restrict key;
	const layer_model_item_s *restrict v;
	if (layer_model_item_iprint(&item->item, log))
	{
		mlog_printf(log->input, "@(%s) count = %u\n", t->cname?t->cname:"-", item->object->vslot.number);
		if (layer_log_push(log, 1))
		{
			ov = t->o?layer_model_object_vattr(t->o):NULL;
			if (ov)
			{
				for (vl = ov->vattr; vl; vl = vl->vattr_next)
				{
					key = vl->vslot->key;
					if ((v = (const layer_model_item_s *) vattr_get_first(item->object, key)))
						layer_model_type_object_iprint_child(v, log, key, '*');
					else
					{
						layer_model_object_find(t->o, key, NULL, NULL, (layer_model_item_s **) &v);
						layer_model_type_object_iprint_child(v, log, key, '~');
					}
				}
			}
			for (vl = item->object->vattr; vl; vl = vl->vattr_next)
			{
				key = vl->vslot->key;
				v = (const layer_model_item_s *) vl->value;
				if (!ov || !vattr_get_vslot(ov, key))
					layer_model_type_object_iprint_child(v, log, key, ':');
			}
			layer_log_pop(log, 1);
		}
	}
}
static d_type_initial(object, layer_model_type_object_param_t)
{
	refer_set_free(t, (refer_free_f) layer_model_type_object_free_func);
	((layer_model_type_object_s *) t)->cname = (refer_string_t) refer_save(pri->cname);
	((layer_model_type_object_s *) t)->cany = (const layer_model_any_s *) refer_save(pri->cany);
	((layer_model_type_object_s *) t)->o = (const layer_model_object_s *) refer_save(pri->o);
	t->create = d_type_function(object, create);
	t->copyto = d_type_function(object, copyto);
	t->iprint = d_type_function(object, iprint);
	return t;
}

const layer_model_type_s* layer_model_type_create__object(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname, const char *restrict oname)
{
	layer_model_type_object_param_t param;
	param.cname = NULL;
	param.cany = NULL;
	param.o = NULL;
	if (cname && !(param.cname = layer_model_find_cname(m, cname, NULL, &param.cany)))
		goto label_fail;
	if (oname && !(param.o = layer_model_find_object(m, oname)))
		goto label_fail;
	return layer_model_type_alloc(name, layer_model_type__object, type_minor, sizeof(layer_model_type_object_s), d_type_function(object, initial), &param);
	label_fail:
	return NULL;
}

// api

const struct vattr_s* layer_model_get_o_vattr(const layer_model_item_s *restrict i)
{
	if (i->type_major == layer_model_type__object)
		return ((layer_model_item_object_s *) i)->object;
	return NULL;
}
layer_model_item_s* layer_model_get_o_child(const layer_model_item_s *restrict i, const char *restrict key)
{
	if (i->type_major == layer_model_type__object && key)
		return (layer_model_item_s *) vattr_get_first(((layer_model_item_object_s *) i)->object, key);
	return NULL;
}
layer_model_item_s* layer_model_get_o_value(const layer_model_item_s *restrict i, const char *restrict key)
{
	layer_model_item_s *restrict item;
	const layer_model_object_s *restrict o;
	if (i->type_major == layer_model_type__object && key)
	{
		if ((item = (layer_model_item_s *) vattr_get_first(((layer_model_item_object_s *) i)->object, key)) ||
			((o = ((layer_model_type_object_s *) i->type)->o) &&
				layer_model_object_find(o, key, NULL, NULL, &item) && item))
			return item;
	}
	return NULL;
}
layer_model_item_s* layer_model_set_o_clear(layer_model_item_s *restrict i)
{
	if (i->type_major == layer_model_type__object && (i->item_flag & layer_model_flag__write))
	{
		vattr_clear(((layer_model_item_object_s *) i)->object);
		return i;
	}
	return NULL;
}
layer_model_item_s* layer_model_set_o_delete(layer_model_item_s *restrict i, const char *restrict key)
{
	if (i->type_major == layer_model_type__object && (i->item_flag & layer_model_flag__write) && key)
	{
		vattr_delete(((layer_model_item_object_s *) i)->object, key);
		return i;
	}
	return NULL;
}
layer_model_item_s* layer_model_set_o_insert(layer_model_item_s *restrict i, const char *restrict key, layer_model_item_s *restrict item)
{
	const layer_model_type_object_s *restrict t;
	t = (const layer_model_type_object_s *) i->type;
	if (i->type_major == layer_model_type__object && (i->item_flag & layer_model_flag__write) &&
		key && item && layer_model_type_object_inherit_type(t, key, item->type) &&
		!vattr_get_vslot(((layer_model_item_object_s *) i)->object, key) &&
		!(item->item_flag & layer_model_flag__child) &&
		vattr_insert_tail(((layer_model_item_object_s *) i)->object, key, item))
	{
		item->item_flag |= layer_model_flag__child;
		return item;
	}
	return NULL;
}
layer_model_item_s* layer_model_set_o_create(layer_model_item_s *restrict i, const char *restrict key, const char *restrict type)
{
	const layer_model_type_object_s *restrict t;
	layer_model_item_s *restrict item;
	t = (const layer_model_type_object_s *) i->type;
	if (i->type_major == layer_model_type__object && (i->item_flag & layer_model_flag__write) &&
		key && layer_model_type_object_inherit_name(t, key, type) &&
		!vattr_get_vslot(((layer_model_item_object_s *) i)->object, key) &&
		(item = layer_model_create_item(((layer_model_item_object_s *) i)->model, type)))
	{
		if (vattr_insert_tail(((layer_model_item_object_s *) i)->object, key, item))
		{
			item->item_flag |= layer_model_flag__child;
			refer_free(item);
			return item;
		}
		refer_free(item);
	}
	return NULL;
}
