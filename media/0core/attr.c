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

static void media_attr_free_attr_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value) refer_free(vl->value);
}

static void media_attr_free_func(struct media_attr_s *restrict r)
{
	hashmap_uini(&r->attr);
}

struct media_attr_s* media_attr_alloc(const struct media_attr_s *restrict src)
{
	struct media_attr_s *restrict r;
	if ((r = (struct media_attr_s *) refer_alloz(sizeof(struct media_attr_s))))
	{
		if (hashmap_init(&r->attr))
		{
			refer_set_free(r, (refer_free_f) media_attr_free_func);
			if (!src || media_attr_copy(r, src, media_attr_copy__replace))
				return r;
		}
		refer_free(r);
	}
	return NULL;
}

static void media_attr_copy_replace_func(hashmap_vlist_t *restrict vl, hashmap_t **restrict pdst)
{
	hashmap_t *restrict hm;
	if ((hm = *pdst) && vl->name)
	{
		if (hashmap_set_name(hm, vl->name, vl->value, vl->free))
			refer_save(vl->value);
		else *pdst = NULL;
	}
}

static void media_attr_copy_ignore_func(hashmap_vlist_t *restrict vl, hashmap_t **restrict pdst)
{
	hashmap_t *restrict hm;
	if ((hm = *pdst) && vl->name && !hashmap_find_name(hm, vl->name))
	{
		if (hashmap_set_name(hm, vl->name, vl->value, vl->free))
			refer_save(vl->value);
		else *pdst = NULL;
	}
}

struct media_attr_s* media_attr_copy(struct media_attr_s *restrict dst, const struct media_attr_s *restrict src, enum media_attr_copy_t ctype)
{
	hashmap_t *hm;
	hm = &dst->attr;
	switch (ctype)
	{
		case media_attr_copy__replace:
			hashmap_call(&src->attr, (hashmap_func_call_f) media_attr_copy_replace_func, &hm);
			break;
		case media_attr_copy__ignore:
			hashmap_call(&src->attr, (hashmap_func_call_f) media_attr_copy_ignore_func, &hm);
			break;
		default: return NULL;
	}
	return hm?dst:NULL;
}

void media_attr_clear(struct media_attr_s *restrict attr)
{
	hashmap_clear(&attr->attr);
}

void media_attr_unset(struct media_attr_s *restrict attr, const char *restrict name)
{
	hashmap_delete_name(&attr->attr, name);
}

struct media_attr_s* media_attr_set(struct media_attr_s *restrict attr, const char *restrict name, enum media_attr_type_t type, union media_attr_value_t value)
{
	const struct media_attr_item_s *restrict item;
	if (name && (item = media_attr_item_alloc(type, value)))
	{
		if (hashmap_set_name(&attr->attr, name, item, media_attr_free_attr_func))
			return attr;
		refer_free(item);
	}
	return NULL;
}

const struct media_attr_s* media_attr_get(const struct media_attr_s *restrict attr, const char *restrict name, enum media_attr_type_t type, union media_attr_value_t *restrict value)
{
	const struct media_attr_item_s *restrict item;
	if (name && (item = (const struct media_attr_item_s *) hashmap_get_name(&attr->attr, name)) &&
		item->type == type)
	{
		if (value) *value = item->value;
		return attr;
	}
	return NULL;
}
