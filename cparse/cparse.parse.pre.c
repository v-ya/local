#include "cparse.type.h"
#include <hashmap.h>

typedef struct cparse_parse_pre_s {
	cparse_parse_s parse;
	hashmap_t special_proxy;        // => (cparse_parse_s)
	tparse_tword_edge_s *we;        // pre name
	tparse_tstring_cache_s *cache;  // build pre name
	tparse_tstring_s *ts;           // get pre value
} cparse_parse_pre_s;

static tparse_tword_edge_s* cparse_inner_alloc_parse_pre_tword(void)
{
	tparse_tword_edge_s *restrict r;
	r = tparse_tword_edge_alloc();
	if (r)
	{
		uintptr_t i;
		for (i = 'a'; i <= 'z'; ++i)
			r->edge[i] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
	}
	return r;
}

static tparse_tstring_trigger_s* cparse_inner_alloc_parse_pre_tstring_tf_0x0a(tparse_tstring_trigger_s *restrict trigger, tparse_tstring_value_t *restrict value, const char *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	// '\n'
	tparse_tstring_value_rollback(value, 1);
	tparse_tstring_value_finally(value);
	return trigger;
}

static tparse_tstring_trigger_s* cparse_inner_alloc_parse_pre_tstring_tf_0x0d(tparse_tstring_trigger_s *restrict trigger, tparse_tstring_value_t *restrict value, const char *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	// '\r'
	tparse_tstring_value_rollback(value, 1);
	return trigger;
}

static tparse_tstring_trigger_s* cparse_inner_alloc_parse_pre_tstring_tf_0x5c(tparse_tstring_trigger_s *restrict trigger, tparse_tstring_value_t *restrict value, const char *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	// '\\'
	uintptr_t p;
	p = *pos;
	if (p < size && data[p] == '\r')
		++p;
	if (p < size && data[p] == '\n')
		++p;
	if (p != *pos)
	{
		tparse_tstring_value_rollback(value, 1);
		if (!tparse_tstring_value_append(value, " ", 1))
			goto label_fail;
		*pos = p;
	}
	return trigger;
	label_fail:
	return NULL;
}

static tparse_tstring_s* cparse_inner_alloc_parse_pre_tstring(void)
{
	tparse_tstring_s *restrict r;
	r = tparse_tstring_alloc_empty(tparse_tmapping_alloc_single);
	if (r)
	{
		if (
			tparse_tstring_set_trigger_func(r, tparse_tstring_trigger_inner, "\n", cparse_inner_alloc_parse_pre_tstring_tf_0x0a) &&
			tparse_tstring_set_trigger_func(r, tparse_tstring_trigger_inner, "\r", cparse_inner_alloc_parse_pre_tstring_tf_0x0d) &&
			tparse_tstring_set_trigger_func(r, tparse_tstring_trigger_inner, "\\", cparse_inner_alloc_parse_pre_tstring_tf_0x5c)
		) return r;
		refer_free(r);
	}
	return NULL;
}

static void cparse_parse_pre_free_func(cparse_parse_pre_s *restrict r)
{
	hashmap_uini(&r->special_proxy);
	if (r->we) refer_free(r->we);
	if (r->cache) refer_free(r->cache);
	if (r->ts) refer_free(r->ts);
}

static cparse_parse_s* cparse_inner_parse_pre(cparse_parse_pre_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context)
{
	const char *restrict key, *restrict data;
	uintptr_t pos, length;
	cparse_parse_s *restrict proxy;
	cparse_parse_s *ret;
	ret = NULL;
	pos = context->pos - 1;
	context->pos = tparse_tword_skip(pri->we, context->data, context->size, context->pos);
	key = tparse_tstring_cache_get(pri->cache, context->data + pos, context->pos - pos);
	if (key)
	{
		tparse_tstring_clear(pri->ts);
		if (tparse_tstring_transport(pri->ts, context->data, context->size, context->pos, &context->pos) &&
			(data = tparse_tstring_get_data(pri->ts, &length)))
		{
			proxy = (cparse_parse_s *) hashmap_get_name(&pri->special_proxy, key);
			if (proxy)
			{
				// proxy to call
				cparse_inst_context_t c;
				c = *context;
				c.data = data;
				c.size = length;
				c.pos = 0;
				if (proxy->parse(proxy, inst, &c))
					ret = &pri->parse;
			}
			else
			{
				cparse_value_s *restrict value;
				value = cparse_inner_alloc_value_nstring(cparse_value$pretreat, data, length);
				if (value)
				{
					if (vattr_insert_tail(context->scope, key, value))
						ret = &pri->parse;
					refer_free(value);
				}
			}
		}
	}
	return ret;
}

static void cparse_inner_alloc_parse_pre_proxy_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		refer_free(vl->value);
}

static cparse_parse_s* cparse_inner_alloc_parse_pre_set_proxy_func(cparse_parse_pre_s *restrict r, const char *restrict name, cparse_parse_f func)
{
	cparse_parse_s *restrict v;
	if ((v = (cparse_parse_s *) refer_alloc(sizeof(cparse_parse_s))))
	{
		v->parse = func;
		if (hashmap_set_name(&r->special_proxy, name, v, cparse_inner_alloc_parse_pre_proxy_free_func))
			return &r->parse;
		refer_free(v);
	}
	return NULL;
}

cparse_parse_s* cparse_inner_parse_pre_include(cparse_parse_s *restrict pri, cparse_inst_s *restrict inst, cparse_inst_context_t *restrict context);

cparse_parse_s* cparse_inner_alloc_parse_pre(void)
{
	cparse_parse_pre_s *restrict r;
	if ((r = (cparse_parse_pre_s *) refer_alloz(sizeof(cparse_parse_pre_s))))
	{
		refer_set_free(r, (refer_free_f) cparse_parse_pre_free_func);
		r->parse.parse = (cparse_parse_f) cparse_inner_parse_pre;
		if (hashmap_init(&r->special_proxy) &&
			(r->we = cparse_inner_alloc_parse_pre_tword()) &&
			(r->cache = tparse_tstring_cache_alloc()) &&
			(r->ts = cparse_inner_alloc_parse_pre_tstring()) &&
			cparse_inner_alloc_parse_pre_set_proxy_func(r, "#include", cparse_inner_parse_pre_include)
		) return &r->parse;
		refer_free(r);
	}
	return NULL;
}

