#include "gvcx.model.h"

typedef struct gvcx_model_type_object_s gvcx_model_type_object_s;
typedef struct gvcx_model_item_object_s gvcx_model_item_object_s;
typedef struct gvcx_model_type_object_param_t gvcx_model_type_object_param_t;

struct gvcx_model_type_object_s {
	gvcx_model_type_s type;
	refer_string_t cname;
	const gvcx_model_any_s *cany;
	const gvcx_model_object_s *o;
};

struct gvcx_model_item_object_s {
	gvcx_model_item_s item;
	const gvcx_model_s *model;
	vattr_s *object;
};

struct gvcx_model_type_object_param_t {
	refer_string_t cname;
	const gvcx_model_any_s *cany;
	const gvcx_model_object_s *o;
};

static const char* gvcx_model_type_object_inherit_name(const gvcx_model_type_object_s *restrict o, const char *restrict key, const char *restrict name)
{
	refer_string_t cname;
	const gvcx_model_any_s *restrict cany;
	if (!o->o || !gvcx_model_object_find(o->o, key, &cname, &cany, NULL))
		cname = o->cname, cany = o->cany;
	return gvcx_model_type_inherit_name(cname, cany, name);
}

static const gvcx_model_type_s* gvcx_model_type_object_inherit_type(const gvcx_model_type_object_s *restrict o, const char *restrict key, const gvcx_model_type_s *restrict type)
{
	refer_string_t cname;
	const gvcx_model_any_s *restrict cany;
	if (!o->o || !gvcx_model_object_find(o->o, key, &cname, &cany, NULL))
		cname = o->cname, cany = o->cany;
	return gvcx_model_type_inherit_type(cname, cany, type);
}

static void gvcx_model_type_object_iprint_child(const gvcx_model_item_s *restrict item, gvcx_log_s *restrict log, const char *restrict key, int ctype)
{
	mlog_printf(log->input, "\"%s\"%c ", key, ctype);
	if (item) gvcx_model_iprint_item(log, item);
	else mlog_printf(log->input, "-\n");
}

static void gvcx_model_type_object_free_func(gvcx_model_type_object_s *restrict r)
{
	if (r->o) refer_free(r->o);
	if (r->cany) refer_free(r->cany);
	if (r->cname) refer_free(r->cname);
	gvcx_model_type_free_func(&r->type);
}

static void gvcx_model_item_object_free_func(gvcx_model_item_object_s *restrict r)
{
	if (r->object) refer_free(r->object);
	if (r->model) refer_free(r->model);
	gvcx_model_item_free_func(&r->item);
}

static d_type_create(object, gvcx_model_type_object_s)
{
	gvcx_model_item_object_s *restrict r;
	if ((r = (gvcx_model_item_object_s *) gvcx_model_item_alloc(sizeof(gvcx_model_item_object_s), &t->type)))
	{
		refer_set_free(r, (refer_free_f) gvcx_model_item_object_free_func);
		r->model = (const gvcx_model_s *) refer_save(m);
		if ((r->object = vattr_alloc()))
			return &r->item;
		refer_free(r);
	}
	return NULL;
}
static d_type_copyto(object, gvcx_model_type_object_s, gvcx_model_item_object_s)
{
	const gvcx_model_item_s *restrict si;
	gvcx_model_item_s *restrict di;
	vattr_vlist_t *restrict vl;
	for (vl = src->object->vattr; vl; vl = vl->vattr_next)
	{
		si = (const gvcx_model_item_s *) vl->value;
		if (!(di = gvcx_model_set_o_create(&dst->item, vl->vslot->key, si->type->name)))
			goto label_fail;
		if (!gvcx_model_copyto_item(di, si))
			goto label_fail;
	}
	return &dst->item;
	label_fail:
	return NULL;
}
static d_type_iprint(object, gvcx_model_type_object_s, gvcx_model_item_object_s)
{
	const vattr_s *restrict ov;
	vattr_vlist_t *restrict vl;
	const char *restrict key;
	const gvcx_model_item_s *restrict v;
	if (gvcx_model_item_iprint(&item->item, log))
	{
		mlog_printf(log->input, "@(%s) count = %u\n", t->cname?t->cname:"-", item->object->vslot.number);
		if (gvcx_log_push(log, 1))
		{
			ov = t->o?gvcx_model_object_vattr(t->o):NULL;
			if (ov)
			{
				for (vl = ov->vattr; vl; vl = vl->vattr_next)
				{
					key = vl->vslot->key;
					if ((v = (const gvcx_model_item_s *) vattr_get_first(item->object, key)))
						gvcx_model_type_object_iprint_child(v, log, key, '*');
					else
					{
						gvcx_model_object_find(t->o, key, NULL, NULL, (gvcx_model_item_s **) &v);
						gvcx_model_type_object_iprint_child(v, log, key, '~');
					}
				}
			}
			for (vl = item->object->vattr; vl; vl = vl->vattr_next)
			{
				key = vl->vslot->key;
				v = (const gvcx_model_item_s *) vl->value;
				if (!ov || !vattr_get_vslot(ov, key))
					gvcx_model_type_object_iprint_child(v, log, key, ':');
			}
			gvcx_log_pop(log, 1);
		}
	}
}
static d_type_initial(object, gvcx_model_type_object_param_t)
{
	refer_set_free(t, (refer_free_f) gvcx_model_type_object_free_func);
	((gvcx_model_type_object_s *) t)->cname = (refer_string_t) refer_save(pri->cname);
	((gvcx_model_type_object_s *) t)->cany = (const gvcx_model_any_s *) refer_save(pri->cany);
	((gvcx_model_type_object_s *) t)->o = (const gvcx_model_object_s *) refer_save(pri->o);
	t->create = d_type_function(object, create);
	t->copyto = d_type_function(object, copyto);
	t->iprint = d_type_function(object, iprint);
	return t;
}

const gvcx_model_type_s* gvcx_model_type_create__object(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname, const char *restrict oname)
{
	gvcx_model_type_object_param_t param;
	param.cname = NULL;
	param.cany = NULL;
	param.o = NULL;
	if (cname && !(param.cname = gvcx_model_find_cname(m, cname, NULL, &param.cany)))
		goto label_fail;
	if (oname && !(param.o = gvcx_model_find_object(m, oname)))
		goto label_fail;
	return gvcx_model_type_alloc(name, gvcx_model_type__object, type_minor, sizeof(gvcx_model_type_object_s), d_type_function(object, initial), &param);
	label_fail:
	return NULL;
}

// api

gvcx_model_item_s* gvcx_model_get_o_child(const gvcx_model_item_s *restrict i, const char *restrict key)
{
	if (i->type_major == gvcx_model_type__object && key)
		return (gvcx_model_item_s *) vattr_get_first(((gvcx_model_item_object_s *) i)->object, key);
	return NULL;
}
gvcx_model_item_s* gvcx_model_get_o_value(const gvcx_model_item_s *restrict i, const char *restrict key)
{
	gvcx_model_item_s *restrict item;
	const gvcx_model_object_s *restrict o;
	if (i->type_major == gvcx_model_type__object && key)
	{
		if ((item = (gvcx_model_item_s *) vattr_get_first(((gvcx_model_item_object_s *) i)->object, key)) ||
			((o = ((gvcx_model_type_object_s *) i->type)->o) &&
				gvcx_model_object_find(o, key, NULL, NULL, &item) && item))
			return item;
	}
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_o_clear(gvcx_model_item_s *restrict i)
{
	if (i->type_major == gvcx_model_type__object && (i->item_flag & gvcx_model_flag__write))
	{
		vattr_clear(((gvcx_model_item_object_s *) i)->object);
		return i;
	}
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_o_delete(gvcx_model_item_s *restrict i, const char *restrict key)
{
	if (i->type_major == gvcx_model_type__object && (i->item_flag & gvcx_model_flag__write) && key)
	{
		vattr_delete(((gvcx_model_item_object_s *) i)->object, key);
		return i;
	}
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_o_insert(gvcx_model_item_s *restrict i, const char *restrict key, gvcx_model_item_s *restrict item)
{
	const gvcx_model_type_object_s *restrict t;
	t = (const gvcx_model_type_object_s *) i->type;
	if (i->type_major == gvcx_model_type__object && (i->item_flag & gvcx_model_flag__write) &&
		key && item && gvcx_model_type_object_inherit_type(t, key, item->type) &&
		!vattr_get_vslot(((gvcx_model_item_object_s *) i)->object, key) &&
		!(item->item_flag & gvcx_model_flag__child) &&
		vattr_insert_tail(((gvcx_model_item_object_s *) i)->object, key, item))
	{
		item->item_flag |= gvcx_model_flag__child;
		return item;
	}
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_o_create(gvcx_model_item_s *restrict i, const char *restrict key, const char *restrict type)
{
	const gvcx_model_type_object_s *restrict t;
	gvcx_model_item_s *restrict item;
	t = (const gvcx_model_type_object_s *) i->type;
	if (i->type_major == gvcx_model_type__object && (i->item_flag & gvcx_model_flag__write) &&
		key && gvcx_model_type_object_inherit_name(t, key, type) &&
		!vattr_get_vslot(((gvcx_model_item_object_s *) i)->object, key) &&
		(item = gvcx_model_create_item(((gvcx_model_item_object_s *) i)->model, type)))
	{
		if (vattr_insert_tail(((gvcx_model_item_object_s *) i)->object, key, item))
		{
			item->item_flag |= gvcx_model_flag__child;
			refer_free(item);
			return item;
		}
		refer_free(item);
	}
	return NULL;
}
