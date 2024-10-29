#include "miko.log.h"

static miko_log_item_t* miko_log_item_initial(miko_log_item_t *restrict v)
{
	switch (v->type)
	{
		case miko_log_type__item:
			refer_save(v->data.item.key);
			refer_save(v->data.item.value);
			return v;
		case miko_log_type__inode:
			refer_save(v->data.inode.name);
			return v;
		default:
			return NULL;
	}
}

static void miko_log_item_finally(miko_log_item_t *restrict v)
{
	switch (v->type)
	{
		case miko_log_type__item:
			refer_ck_free(v->data.item.key);
			refer_ck_free(v->data.item.value);
			break;
		case miko_log_type__inode:
			refer_ck_free(v->data.inode.name);
			break;
		default:
			break;
	}
}

static void miko_log_free_func(miko_log_s *restrict r)
{
	refer_ck_free(r->log_array);
	refer_ck_free(r->log_string);
}

miko_log_s* miko_log_alloc(void)
{
	miko_log_s *restrict r;
	if ((r = (miko_log_s *) refer_alloz(sizeof(miko_log_s))))
	{
		refer_hook_free(r, log);
		r->log_array = miko_vector_alloc(sizeof(miko_log_item_t),
			(miko_vector_initial_f) miko_log_item_initial,
			(miko_vector_finally_f) miko_log_item_finally);
		r->log_string = miko_strpool_alloc();
		r->limit = miko_log_level__debug;
		if (r->log_array && r->log_string)
			return r;
		refer_free(r);
	}
	return NULL;
}

void miko_log_set_limit(miko_log_s *restrict r, miko_log_level_t limit)
{
	r->limit = limit;
}

miko_log_s* miko_log_add(miko_log_s *restrict r, miko_log_level_t level, refer_string_t key, refer_string_t value)
{
	miko_log_item_t item;
	if (r && level <= r->limit)
	{
		item.type = miko_log_type__item;
		item.level = level;
		item.data.item.key = key;
		item.data.item.value = value;
		if (miko_vector_push(r->log_array, &item, 1))
			return r;
	}
	return NULL;
}

miko_log_s* miko_log_add_key(miko_log_s *restrict r, miko_log_level_t level, const char *restrict key, refer_string_t value)
{
	refer_string_t k;
	if (r && level <= r->limit)
	{
		k = miko_strpool_save(r->log_string, key);
		r = miko_log_add(r, level, k, value);
		refer_ck_free(k);
		return r;
	}
	return NULL;
}

miko_log_s* miko_log_add_value(miko_log_s *restrict r, miko_log_level_t level, refer_string_t key, const char *restrict value)
{
	refer_string_t v;
	if (r && level <= r->limit)
	{
		v = miko_strpool_save(r->log_string, value);
		r = miko_log_add(r, level, key, v);
		refer_ck_free(v);
		return r;
	}
	return NULL;
}

miko_log_s* miko_log_add_key_value(miko_log_s *restrict r, miko_log_level_t level, const char *restrict key, const char *restrict value)
{
	refer_string_t k;
	refer_string_t v;
	if (r && level <= r->limit)
	{
		k = miko_strpool_save(r->log_string, key);
		v = miko_strpool_save(r->log_string, value);
		r = miko_log_add(r, level, k, v);
		refer_ck_free(k);
		refer_ck_free(v);
		return r;
	}
	return NULL;
}

uintptr_t miko_log_into(miko_log_s *restrict r, refer_string_t name)
{
	miko_log_item_t item;
	uintptr_t result;
	if (r)
	{
		item.type = miko_log_type__inode;
		item.level = miko_log_level_max;
		item.data.inode.name = name;
		item.data.inode.count = 0;
		result = miko_vector_count(r->log_array);
		if (miko_vector_push(r->log_array, &item, 1))
			return result;
	}
	return ~(uintptr_t) 0;
}

uintptr_t miko_log_into_name(miko_log_s *restrict r, const char *restrict name)
{
	refer_string_t n;
	uintptr_t result;
	if (r)
	{
		n = miko_strpool_save(r->log_string, name);
		result = miko_log_into(r, n);
		refer_ck_free(n);
		return result;
	}
	return ~(uintptr_t) 0;
}

void miko_log_back(miko_log_s *restrict r, uintptr_t into_result)
{
	miko_log_item_t *restrict item;
	uintptr_t count;
	if (r && ~into_result && (item = (miko_log_item_t *) miko_vector_index(r->log_array, into_result)) &&
		item->type == miko_log_type__inode && !item->data.inode.count)
	{
		into_result += 1;
		if ((count = miko_vector_count(r->log_array)) > into_result)
			item->data.inode.count = count - into_result;
		else miko_vector_pop(r->log_array, 1);
	}
}

static void miko_log_print_array(mlog_s *restrict mlog, const miko_log_item_t *restrict p, uintptr_t n, uint32_t indent)
{
	static const char *const _empty_ = "";
	while (n)
	{
		switch (p->type)
		{
			case miko_log_type__item:
				mlog_printf(mlog, "%*s" "%s: <%s>\n", indent, _empty_,
					p->data.item.key?p->data.item.key:_empty_,
					p->data.item.value?p->data.item.value:_empty_);
				break;
			case miko_log_type__inode:
				mlog_printf(mlog, "%*s" "(%s)\n", indent, _empty_,
					p->data.inode.name?p->data.inode.name:_empty_);
				if (p->data.inode.count < n)
				{
					miko_log_print_array(mlog, p + 1, p->data.inode.count, indent + 4);
					n -= p->data.inode.count;
					p += p->data.inode.count;
				}
				break;
			default:
				break;
		}
		p += 1;
		n -= 1;
	}
}

void miko_log_print(miko_log_s *restrict r, mlog_s *restrict mlog)
{
	const miko_log_item_t *restrict p;
	uintptr_t n;
	if (r && mlog && (p = miko_vector_map(r->log_array, 0, miko_vector_auto, &n)))
		miko_log_print_array(mlog, p, n, 0);
}
