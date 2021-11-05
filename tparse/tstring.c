#include "tstring.h"
#include "tmapping.h"
#include <exbuffer.h>
#include <string.h>

struct tparse_tstring_value_t {
	exbuffer_t value;
	uintptr_t finally;
};

struct tparse_tstring_s {
	tparse_tmapping_s *mapping;
	tparse_tstring_trigger_s *head;
	tparse_tstring_trigger_s *tail;
	tparse_tstring_value_t value;
};

struct tparse_tstring_cache_s {
	exbuffer_t cache;
};

static inline void* tparse_tstring_value_inline_append_char(tparse_tstring_value_t *restrict value, char c)
{
	char *p;
	if (value->value.used < value->value.size)
	{
		p = (char *) value->value.data;
		p[value->value.used++] = c;
		label_okay:
		return p;
	}
	else if ((p = (char *) exbuffer_append(&value->value, &c, 1)))
		goto label_okay;
	return NULL;
}

tparse_tstring_value_t* tparse_tstring_value_append(tparse_tstring_value_t *restrict value, const char *restrict string, uintptr_t length)
{
	if (!length) length = strlen(string);
	if ((length == 1)?tparse_tstring_value_inline_append_char(value, *string):exbuffer_append(&value->value, string, length))
		return value;
	return NULL;
}

void tparse_tstring_value_rollback(tparse_tstring_value_t *restrict value, uintptr_t n)
{
	if (value->value.used >= n)
		value->value.used -= n;
	else value->value.used = 0;
}

void tparse_tstring_value_finally(tparse_tstring_value_t *restrict value)
{
	value->finally = 1;
}

static void tparse_tstring_free_func(tparse_tstring_s *restrict r)
{
	if (r->mapping)
		refer_free(r->mapping);
	if (r->head)
		refer_free(r->head);
	if (r->tail)
		refer_free(r->tail);
	exbuffer_uini(&r->value.value);
}

tparse_tstring_s* tparse_tstring_alloc_empty(struct tparse_tmapping_s* (*mp_alloc_func)(void))
{
	tparse_tstring_s *restrict r;
	if (mp_alloc_func && (r = (tparse_tstring_s *) refer_alloz(sizeof(tparse_tstring_s))))
	{
		refer_set_free(r, (refer_free_f) tparse_tstring_free_func);
		if (exbuffer_init(&r->value.value, 0) &&
			(r->mapping = mp_alloc_func()))
			return r;
		refer_free(r);
	}
	return NULL;
}

tparse_tstring_s* tparse_tstring_set_trigger(tparse_tstring_s *restrict ts, tparse_tstring_trigger_t type, const char *restrict inner_name, tparse_tstring_trigger_s *trigger)
{
	tparse_tstring_trigger_s **restrict p;
	if (trigger && trigger->trigger)
	{
		switch (type)
		{
			case tparse_tstring_trigger_head:
				p = &ts->head;
				goto label_replace;
			case tparse_tstring_trigger_inner:
				if (inner_name && ts->mapping->add(ts->mapping, inner_name, trigger))
					goto label_okay;
				goto label_fail;
			case tparse_tstring_trigger_tail:
				p = &ts->tail;
				goto label_replace;
		}
	}
	label_fail:
	return NULL;
	label_replace:
	if (*p) refer_free(*p);
	*p = (tparse_tstring_trigger_s *) refer_save(trigger);
	label_okay:
	return ts;
}

tparse_tstring_s* tparse_tstring_set_trigger_func(tparse_tstring_s *restrict ts, tparse_tstring_trigger_t type, const char *restrict inner_name, tparse_tstring_trigger_f trigger_func)
{
	tparse_tstring_trigger_s *restrict trigger;
	if (trigger_func && (trigger = (tparse_tstring_trigger_s *) refer_alloc(sizeof(tparse_tstring_trigger_s))))
	{
		ts = tparse_tstring_set_trigger(ts, type, inner_name, trigger);
		refer_free(trigger);
		return ts;
	}
	return NULL;
}

void tparse_tstring_clear(tparse_tstring_s *restrict ts)
{
	ts->mapping->clear(ts->mapping);
	ts->value.value.used = 0;
}

tparse_tstring_s* tparse_tstring_transport(tparse_tstring_s *restrict ts, const char *restrict data, uintptr_t size, uintptr_t pos, uintptr_t *restrict rpos)
{
	tparse_tstring_trigger_s *restrict t;
	tparse_tmapping_s *restrict mp;
	mp = ts->mapping;
	ts->value.finally = 0;
	if ((t = ts->head))
	{
		if (!t->trigger(t, &ts->value, data, size, &pos))
			goto label_fail;
	}
	while (pos < size && ts->value.finally)
	{
		if (!tparse_tstring_value_inline_append_char(&ts->value, data[pos]))
			goto label_fail;
		t = (tparse_tstring_trigger_s *) mp->test(mp, data[pos]);
		++pos;
		if (t)
		{
			if (!t->trigger(t, &ts->value, data, size, &pos))
				goto label_fail;
		}
	}
	if ((t = ts->tail))
	{
		if (!t->trigger(t, &ts->value, data, size, &pos))
			goto label_fail;
	}
	if (ts->value.finally)
	{
		label_return:
		if (rpos) *rpos = pos;
		return ts;
	}
	label_fail:
	ts = NULL;
	goto label_return;
}

const char* tparse_tstring_get_data(tparse_tstring_s *restrict ts, uintptr_t *restrict length)
{
	if (length)
		*length = ts->value.value.used;
	return (const char *) ts->value.value.data;
}

const char* tparse_tstring_get_string(tparse_tstring_s *restrict ts, uintptr_t *restrict length)
{
	char *p;
	uintptr_t n;
	if ((p = (char *) exbuffer_need(&ts->value.value, (n = ts->value.value.used) + 1)))
	{
		p[n] = 0;
		if (length)
			*length = n;
		return p;
	}
	return NULL;
}

static void tparse_tstring_cache_free_func(tparse_tstring_cache_s *restrict r)
{
	exbuffer_uini(&r->cache);
}

tparse_tstring_cache_s* tparse_tstring_cache_alloc(void)
{
	tparse_tstring_cache_s *restrict r;
	if ((r = (tparse_tstring_cache_s *) refer_alloc(sizeof(tparse_tstring_cache_s))))
	{
		if (exbuffer_init(&r->cache, 0))
		{
			refer_set_free(r, (refer_free_f) tparse_tstring_cache_free_func);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

const char* tparse_tstring_cache_get(tparse_tstring_cache_s *restrict cache, const char *restrict text, uintptr_t length)
{
	char *p;
	if ((p = (char *) exbuffer_need(&cache->cache, length + 1)))
	{
		memcpy(p, text, length);
		p[length] = 0;
	}
	return p;
}
