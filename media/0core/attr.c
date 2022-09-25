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
	if (r->judge) refer_free(r->judge);
	if (r->judge_inst) refer_free(r->judge_inst);
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

static hashmap_vlist_t* media_attr_link_value(struct media_attr_s *restrict attr, const char *restrict name, const struct media_attr_item_s *restrict value)
{
	hashmap_vlist_t *restrict vl;
	media_attr_judge_f jf;
	vl = NULL;
	jf = NULL;
	if (attr->judge) jf = (media_attr_judge_f) hashmap_get_name(&attr->judge->judge, name);
	if (!jf || jf(attr, value))
	{
		if ((vl = hashmap_set_name(&attr->attr, name, value, media_attr_free_attr_func)))
			refer_save(value);
		else
		{
			hashmap_delete_name(&attr->attr, name);
			if (jf) jf(attr, NULL);
		}
	}
	return vl;
}

static void media_attr_copy_replace_func(hashmap_vlist_t *restrict vl, struct media_attr_s **restrict pdst)
{
	if (*pdst && vl->name)
	{
		if (!media_attr_link_value(*pdst, vl->name, (const struct media_attr_item_s *) vl->value))
			*pdst = NULL;
	}
}

static void media_attr_copy_ignore_func(hashmap_vlist_t *restrict vl, struct media_attr_s **restrict pdst)
{
	if (*pdst && vl->name && !hashmap_find_name(&(*pdst)->attr, vl->name))
	{
		if (!media_attr_link_value(*pdst, vl->name, (const struct media_attr_item_s *) vl->value))
			*pdst = NULL;
	}
}

struct media_attr_s* media_attr_copy(struct media_attr_s *restrict dst, const struct media_attr_s *restrict src, enum media_attr_copy_t ctype)
{
	struct media_attr_s *rr;
	rr = dst;
	switch (ctype)
	{
		case media_attr_copy__replace:
			hashmap_call(&src->attr, (hashmap_func_call_f) media_attr_copy_replace_func, &rr);
			break;
		case media_attr_copy__ignore:
			hashmap_call(&src->attr, (hashmap_func_call_f) media_attr_copy_ignore_func, &rr);
			break;
		default: return NULL;
	}
	return rr;
}

void media_attr_set_judge(struct media_attr_s *restrict attr, const struct media_attr_judge_s *restrict judge, refer_t judge_inst)
{
	if (attr->judge) refer_free(attr->judge);
	if (attr->judge_inst) refer_free(attr->judge_inst);
	attr->judge = (const struct media_attr_judge_s *) refer_save(judge);
	attr->judge_inst = refer_save(judge_inst);
}

void media_attr_clear(struct media_attr_s *restrict attr)
{
	hashmap_clear(&attr->attr);
	if (attr->judge && attr->judge->clear)
		attr->judge->clear(attr, NULL);
}

void media_attr_unset(struct media_attr_s *restrict attr, const char *restrict name)
{
	media_attr_judge_f jf;
	hashmap_delete_name(&attr->attr, name);
	if (attr->judge && (jf = (media_attr_judge_f) hashmap_get_name(&attr->judge->judge, name)))
		jf(attr, NULL);
}

struct media_attr_s* media_attr_set(struct media_attr_s *restrict attr, const char *restrict name, enum media_attr_type_t type, union media_attr_value_t value)
{
	const struct media_attr_item_s *restrict item;
	if (name)
	{
		if ((item = media_attr_item_alloc(type, value)))
		{
			if (!media_attr_link_value(attr, name, item))
				attr = NULL;
			refer_free(item);
			return attr;
		}
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
