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

static const struct media_attr_s* media_attr_set_it(const struct media_attr_s *restrict attr, const struct media_attr_judge_item_s *restrict ji, const struct media_attr_item_s *restrict value)
{
	if (ji)
	{
		if (!value) goto label_fail;
		switch (ji->need)
		{
			case media_attr_judge_need__any: break;
			case media_attr_judge_need__int:
				if (value->type == media_attr_type__int)
					break;
				goto label_fail;
			case media_attr_judge_need__float:
				if (value->type == media_attr_type__float)
					break;
				goto label_fail;
			case media_attr_judge_need__string:
				if (value->type == media_attr_type__string)
					break;
				goto label_fail;
			case media_attr_judge_need__data:
				if (value->type == media_attr_type__data)
					break;
				goto label_fail;
			case media_attr_judge_need__ptr:
				if (value->type == media_attr_type__ptr)
					break;
				goto label_fail;
			case media_attr_judge_need__uint32:
				if (value->type == media_attr_type__int &&
					(int64_t) (uint32_t) value->value.av_int == value->value.av_int)
					break;
				goto label_fail;
			case media_attr_judge_need__sint32:
				if (value->type == media_attr_type__int &&
					(int64_t) (int32_t) value->value.av_int == value->value.av_int)
					break;
				goto label_fail;
			case media_attr_judge_need__uint16:
				if (value->type == media_attr_type__int &&
					(int64_t) (uint16_t) value->value.av_int == value->value.av_int)
					break;
				goto label_fail;
			case media_attr_judge_need__sint16:
				if (value->type == media_attr_type__int &&
					(int64_t) (int16_t) value->value.av_int == value->value.av_int)
					break;
				goto label_fail;
			case media_attr_judge_need__uint8:
				if (value->type == media_attr_type__int &&
					(int64_t) (uint8_t) value->value.av_int == value->value.av_int)
					break;
				goto label_fail;
			case media_attr_judge_need__sint8:
				if (value->type == media_attr_type__int &&
					(int64_t) (int8_t) value->value.av_int == value->value.av_int)
					break;
				goto label_fail;
			default: goto label_fail;
		}
		return ji->set(attr, attr->pri_data, value);
	}
	label_fail:
	return NULL;
}

static inline void media_attr_unset_it(const struct media_attr_s *restrict attr, const struct media_attr_judge_item_s *restrict ji)
{
	if (ji) ji->unset(attr, attr->pri_data);
}

static struct media_attr_s* media_attr_link_value(struct media_attr_s *restrict attr, const char *restrict name, const struct media_attr_item_s *restrict value)
{
	const struct media_attr_judge_item_s *restrict ji;
	switch (value->type)
	{
		case media_attr_type__string:
		case media_attr_type__data:
		case media_attr_type__ptr:
			if (!value->value.av_ptr)
				goto label_fail;
			break;
		default: break;
	}
	ji = NULL;
	if (!attr->judge || media_attr_set_it(attr, ji = (const struct media_attr_judge_item_s *) hashmap_get_name(&attr->judge->judge, name), value))
	{
		if (vattr_set(attr->attr, name, value))
			return attr;
		vattr_delete(attr->attr, name);
		media_attr_unset_it(attr, ji);
	}
	label_fail:
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

static void media_attr_clear_do_func(hashmap_vlist_t *restrict vl, struct media_attr_s *restrict attr)
{
	media_attr_unset_it(attr, (const struct media_attr_judge_item_s *) vl->value);
}

void media_attr_clear(struct media_attr_s *restrict attr)
{
	vattr_clear(attr->attr);
	if (attr->judge)
	{
		hashmap_call(&attr->judge->judge, (hashmap_func_call_f) media_attr_clear_do_func, attr);
		if (attr->judge->clear)
			attr->judge->clear(attr, attr->pri_data);
	}
}

void media_attr_unset(struct media_attr_s *restrict attr, const char *restrict name)
{
	vattr_delete(attr->attr, name);
	if (attr->judge) media_attr_unset_it(attr, (const struct media_attr_judge_item_s *) hashmap_get_name(&attr->judge->judge, name));
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

struct media_attr_judge_s* media_attr_judge_create(media_attr_judge_initial_f initial)
{
	struct media_attr_judge_s *restrict r;
	if (initial && (r = media_attr_judge_alloc()))
	{
		if (initial(r))
			return r;
		refer_free(r);
	}
	return NULL;
}

#include "media.h"

struct media_attr_judge_s* media_attr_judge_set(struct media_attr_judge_s *restrict judge, const char *restrict name, media_attr_set_f sf, media_attr_unset_f uf, enum media_attr_judge_need_t need)
{
	struct media_attr_judge_item_s *restrict item;
	if (name && sf && uf)
	{
		if ((item = (struct media_attr_judge_item_s *) refer_alloc(sizeof(struct media_attr_judge_item_s))))
		{
			item->set = sf;
			item->unset = uf;
			item->need = need;
			if (hashmap_set_name(&judge->judge, name, item, media_hashmap_free_refer_func))
				return judge;
			refer_free(item);
		}
	}
	return NULL;
}

void media_attr_judge_set_extra_clear(struct media_attr_judge_s *restrict judge, media_attr_unset_f uf)
{
	judge->clear = uf;
}
