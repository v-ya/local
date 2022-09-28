#include "attr.h"

// item

static void media_attr_item_free_func(struct media_attr_item_s *restrict r)
{
	switch (r->type)
	{
		case media_attr_type__string:
		case media_attr_type__data:
		case media_attr_type__ptr:
			if (r->value.av_ptr)
				refer_free(r->value.av_ptr);
			break;
		default: break;
	}
}

static const struct media_attr_item_s* media_attr_item_alloc(enum media_attr_type_t type, union media_attr_value_t value)
{
	struct media_attr_item_s *restrict r;
	if ((r = (struct media_attr_item_s *) refer_alloc(sizeof(struct media_attr_item_s))))
	{
		switch (type)
		{
			case media_attr_type__int: r->value.av_int = value.av_int; break;
			case media_attr_type__float: r->value.av_float = value.av_float; break;
			case media_attr_type__string:
			case media_attr_type__data:
			case media_attr_type__ptr: r->value.av_ptr = refer_save(value.av_ptr); break;
			default: goto label_fail;
		}
		r->type = type;
		refer_set_free(r, (refer_free_f) media_attr_item_free_func);
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

// attr

static void media_attr_free_func(struct media_attr_s *restrict r)
{
	if (r->attr) refer_free(r->attr);
	if (r->judge) refer_free(r->judge);
	if (r->pri_data) refer_free(r->pri_data);
}

struct media_attr_s* media_attr_alloc(const struct media_attr_s *restrict src)
{
	struct media_attr_s *restrict r;
	if ((r = (struct media_attr_s *) refer_alloz(sizeof(struct media_attr_s))))
	{
		refer_set_free(r, (refer_free_f) media_attr_free_func);
		if ((r->attr = vattr_alloc()))
		{
			if (!src || media_attr_copy(r, src, media_attr_copy__replace))
				return r;
		}
		refer_free(r);
	}
	return NULL;
}

static struct media_attr_s* media_attr_link_value(struct media_attr_s *restrict attr, const char *restrict name, const struct media_attr_item_s *restrict value)
{
	media_attr_judge_f jf;
	jf = NULL;
	if (attr->judge) jf = (media_attr_judge_f) hashmap_get_name(&attr->judge->judge, name);
	if (!jf || jf(attr, attr->pri_data, value))
	{
		if (vattr_set(attr->attr, name, value))
			return attr;
		vattr_delete(attr->attr, name);
		if (jf) jf(attr, attr->pri_data, NULL);
	}
	return NULL;
}

struct media_attr_s* media_attr_copy(struct media_attr_s *restrict dst, const struct media_attr_s *restrict src, enum media_attr_copy_t ctype)
{
	vattr_vlist_t *restrict vl;
	struct media_attr_s *rr;
	rr = dst;
	switch (ctype)
	{
		case media_attr_copy__replace:
			for (vl = src->attr->vattr; vl; vl = vl->vattr_next)
			{
				if (!media_attr_link_value(dst, vl->vslot->key, (const struct media_attr_item_s *) vl->value))
					rr = NULL;
			}
			break;
		case media_attr_copy__ignore:
			for (vl = src->attr->vattr; vl; vl = vl->vattr_next)
			{
				if (!vattr_get_vslot(dst->attr, vl->vslot->key))
				{
					if (!media_attr_link_value(dst, vl->vslot->key, (const struct media_attr_item_s *) vl->value))
						rr = NULL;
				}
			}
			break;
		default:
			rr = NULL;
			break;
	}
	return rr;
}

void media_attr_set_judge(struct media_attr_s *restrict attr, const struct media_attr_judge_s *restrict judge, refer_t pri_data)
{
	if (attr->judge) refer_free(attr->judge);
	if (attr->pri_data) refer_free(attr->pri_data);
	attr->judge = (const struct media_attr_judge_s *) refer_save(judge);
	attr->pri_data = refer_save(pri_data);
}

void media_attr_clear(struct media_attr_s *restrict attr)
{
	vattr_clear(attr->attr);
	if (attr->judge && attr->judge->clear)
		attr->judge->clear(attr, attr->pri_data, NULL);
}

void media_attr_unset(struct media_attr_s *restrict attr, const char *restrict name)
{
	media_attr_judge_f jf;
	vattr_delete(attr->attr, name);
	if (attr->judge && (jf = (media_attr_judge_f) hashmap_get_name(&attr->judge->judge, name)))
		jf(attr, attr->pri_data, NULL);
}

struct media_attr_s* media_attr_set(struct media_attr_s *restrict attr, const char *restrict name, enum media_attr_type_t type, union media_attr_value_t value)
{
	const struct media_attr_item_s *restrict item;
	if (name)
	{
		if ((item = media_attr_item_alloc(type, value)))
		{
			attr = media_attr_link_value(attr, name, item);
			refer_free(item);
			return attr;
		}
	}
	return NULL;
}

const struct media_attr_s* media_attr_get(const struct media_attr_s *restrict attr, const char *restrict name, enum media_attr_type_t type, union media_attr_value_t *restrict value)
{
	const struct media_attr_item_s *restrict item;
	if (name && (item = (const struct media_attr_item_s *) vattr_get_first(attr->attr, name)) &&
		item->type == type)
	{
		if (value) *value = item->value;
		return attr;
	}
	return NULL;
}

// judge

static void media_attr_judge_free_func(struct media_attr_judge_s *restrict r)
{
	hashmap_uini(&r->judge);
}

struct media_attr_judge_s* media_attr_judge_alloc(void)
{
	struct media_attr_judge_s *restrict r;
	if ((r = (struct media_attr_judge_s *) refer_alloz(sizeof(struct media_attr_judge_s))))
	{
		if (hashmap_init(&r->judge))
		{
			refer_set_free(r, (refer_free_f) media_attr_judge_free_func);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

struct media_attr_judge_s* media_attr_judge_set(struct media_attr_judge_s *restrict judge, const char *restrict name, media_attr_judge_f jf)
{
	if (jf)
	{
		if (name)
		{
			if (hashmap_set_name(&judge->judge, name, jf, NULL))
				goto label_okay;
		}
		else
		{
			judge->clear = jf;
			label_okay:
			return judge;
		}
	}
	return NULL;
}
