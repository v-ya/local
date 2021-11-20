#include "web-transport.h"

#define web_transport_define_timeout_ms  5000

static const char *s_http_header_id_content_length = "content-length";

static transport_s* web_transport_inner_send_http(transport_s *restrict tp, const uhttp_s *restrict http, exbuffer_t *restrict cache, const web_transport_attr_t *restrict attr, transport_attr_t *restrict ta)
{
	char *p;
	uintptr_t n;
	if ((p = uhttp_build(http, cache, &n)) && (!attr || n <= attr->http_head_max_length))
	{
		ta->flags = transport_attr_flag_do_full | transport_attr_flag_modify_timeout;
		if (transport_send(tp, p, n, NULL, ta))
			return tp;
	}
	return NULL;
}

static transport_s* web_transport_inner_send_http_with_body(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict cache, const void *body, uintptr_t body_length, const web_transport_attr_t *restrict attr, transport_attr_t *restrict ta)
{
	if ((!attr || body_length <= attr->http_body_max_length) &&
		(uhttp_find_header_first(http, s_http_header_id_content_length) ||
			uhttp_insert_header_integer(http, "Content-Length", (int64_t) body_length)))
	{
		if (web_transport_inner_send_http(tp, http, cache, attr, ta))
		{
			ta->flags = transport_attr_flag_do_full | transport_attr_flag_modify_timeout;
			if (transport_send(tp, body, body_length, NULL, ta))
				return tp;
		}
	}
	return NULL;
}

static transport_s* web_transport_inner_recv_http(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict cache, const web_transport_attr_t *restrict attr, transport_attr_t *restrict ta)
{
	uhttp_parse_context_t context;
	uintptr_t rn, block;
	void *data;
	uhttp_parse_context_init(&context, 0);
	block = 4096;
	if (attr && block > attr->http_head_max_length)
		block = attr->http_head_max_length;
	if ((data = exbuffer_need(cache, block)))
	{
		uhttp_parse_context_set(&context, data, 0);
		ta->flags = transport_attr_flag_do_some | transport_attr_flag_modify_timeout;
		while (transport_recv(tp, data + context.size, block - context.size, &rn, ta) && rn)
		{
			context.size += rn;
			switch (uhttp_parse_context_try(http, &context))
			{
				case uhttp_parse_status_okay:
					if (context.size > context.pos)
						tp = transport_push_recv(tp, data + context.pos, context.size - context.pos);
					return tp;
				case uhttp_parse_status_wait_header:
				case uhttp_parse_status_wait_line:
					if (context.size < block)
						break;
					if (!attr || block < attr->http_head_max_length)
					{
						block <<= 1;
						if (attr && block > attr->http_head_max_length)
							block = attr->http_head_max_length;
						if (block && (data = exbuffer_need(cache, block)))
						{
							context.data = (const char *) data;
							break;
						}
					}
					// fall through
				default:
					goto label_fail;
			}
		}
	}
	label_fail:
	return NULL;
}

static transport_s* web_transport_inner_recv_body(transport_s *restrict tp, const uhttp_s *restrict http, exbuffer_t *restrict body, const web_transport_attr_t *restrict attr, transport_attr_t *restrict ta)
{
	uintptr_t body_length;
	void *data;
	body->used = 0;
	if ((body_length = (uintptr_t) uhttp_get_header_integer_first(http, s_http_header_id_content_length)))
	{
		if (attr && body_length > attr->http_body_max_length)
			goto label_fail;
		if (!(data = exbuffer_need(body, body_length)))
			goto label_fail;
		ta->flags = transport_attr_flag_do_full | transport_attr_flag_modify_timeout;
		tp = transport_recv(tp, data, body_length, &body_length, ta);
		body->used = body_length;
	}
	return tp;
	label_fail:
	return NULL;
}

transport_s* web_transport_send_http(transport_s *restrict tp, const uhttp_s *restrict http, exbuffer_t *restrict cache, const web_transport_attr_t *restrict attr)
{
	transport_attr_t ta;
	ta.running = attr?attr->running:NULL;
	ta.timeout_ms = attr?attr->http_transport_timeout_ms:web_transport_define_timeout_ms;
	return web_transport_inner_send_http(tp, http, cache, attr, &ta);
}

transport_s* web_transport_send_http_with_body(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict cache, const void *body, uintptr_t body_length, const web_transport_attr_t *restrict attr)
{
	transport_attr_t ta;
	ta.running = attr?attr->running:NULL;
	ta.timeout_ms = attr?attr->http_transport_timeout_ms:web_transport_define_timeout_ms;
	return web_transport_inner_send_http_with_body(tp, http, cache, body, body_length, attr, &ta);
}

transport_s* web_transport_recv_http(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict cache, const web_transport_attr_t *restrict attr)
{
	transport_attr_t ta;
	ta.running = attr?attr->running:NULL;
	ta.timeout_ms = attr?attr->http_transport_timeout_ms:web_transport_define_timeout_ms;
	return web_transport_inner_recv_http(tp, http, cache, attr, &ta);
}

transport_s* web_transport_recv_body(transport_s *restrict tp, const uhttp_s *restrict http, exbuffer_t *restrict body, const web_transport_attr_t *restrict attr)
{
	transport_attr_t ta;
	ta.running = attr?attr->running:NULL;
	ta.timeout_ms = attr?attr->http_transport_timeout_ms:web_transport_define_timeout_ms;
	body->used = 0;
	return web_transport_inner_recv_body(tp, http, body, attr, &ta);
}

transport_s* web_transport_recv_http_with_body(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *cache, exbuffer_t *body, const web_transport_attr_t *restrict attr)
{
	transport_attr_t ta;
	ta.running = attr?attr->running:NULL;
	ta.timeout_ms = attr?attr->http_transport_timeout_ms:web_transport_define_timeout_ms;
	body->used = 0;
	if (web_transport_inner_recv_http(tp, http, cache, attr, &ta) &&
		web_transport_inner_recv_body(tp, http, body, attr, &ta))
		return tp;
	return NULL;
}

transport_s* web_transport_request(transport_s *restrict tp, const web_transport_context_t *restrict context, const web_transport_attr_t *restrict attr)
{
	transport_attr_t ta;
	ta.running = attr?attr->running:NULL;
	ta.timeout_ms = attr?attr->http_transport_timeout_ms:web_transport_define_timeout_ms;
	if (context->response_body)
		context->response_body->used = 0;
	if (!context->cache)
		goto label_fail;
	if (!context->request_http)
		goto label_fail;
	if (!web_transport_inner_send_http_with_body(tp, context->request_http, context->cache, context->request_body, context->request_body_length, attr, &ta))
		goto label_fail;
	if (context->response_http)
	{
		if (!web_transport_inner_recv_http(tp, context->response_http, context->cache, attr, &ta))
			goto label_fail;
		if (context->response_body)
		{
			if (!web_transport_inner_recv_body(tp, context->response_http, context->response_body, attr, &ta))
				goto label_fail;
		}
	}
	return tp;
	label_fail:
	return NULL;
}

// web_transport_poll_http_s

typedef struct web_transport_poll_http_item_s web_transport_poll_http_item_s;

struct web_transport_poll_http_item_s {
	web_transport_poll_http_item_s *next;
	web_transport_poll_http_item_s **p_last;
	uint64_t timestamp_dead;
	transport_s *tp;
	uhttp_s *http;
	uhttp_parse_context_t context;
	exbuffer_t cache;
};

struct web_transport_poll_http_s {
	const uhttp_inst_s *http_inst;
	web_transport_poll_http_item_s *used;
	web_transport_poll_http_item_s *free;
	web_transport_poll_http_item_s *next_loop_first;
	uintptr_t http_limit_size;
};

static void web_transport_poll_http_item_free_func(web_transport_poll_http_item_s *restrict r)
{
	if (r->tp)
		refer_free(r->tp);
	if (r->http)
		refer_free(r->http);
	exbuffer_uini(&r->cache);
}

static web_transport_poll_http_item_s* web_transport_poll_http_item_alloc(void)
{
	web_transport_poll_http_item_s *restrict r;
	if ((r = (web_transport_poll_http_item_s *) refer_alloc(sizeof(web_transport_poll_http_item_s))))
	{
		r->next = NULL;
		r->p_last = NULL;
		r->tp = NULL;
		r->http = NULL;
		if (exbuffer_init(&r->cache, 0))
		{
			refer_set_free(r, (refer_free_f) web_transport_poll_http_item_free_func);
			return r;
		}
		refer_free(r);
	}
	return r;
}

static web_transport_poll_http_item_s* web_transport_poll_http_get_item(web_transport_poll_http_s *restrict r, transport_s *restrict tp)
{
	web_transport_poll_http_item_s *restrict p;
	if ((p = r->free))
	{
		r->free = p->next;
		p->next = NULL;
		p->p_last = NULL;
		label_got:
		if ((p->http = uhttp_alloc(r->http_inst)))
		{
			p->timestamp_dead = transport_timestamp_ms();
			p->tp = (transport_s *) refer_save(tp);
			uhttp_parse_context_init(&p->context, 0);
			return p;
		}
		p->next = r->free;
		r->free = p;
	}
	else if ((p = web_transport_poll_http_item_alloc()))
		goto label_got;
	return NULL;
}

static void web_transport_poll_http_put_item(web_transport_poll_http_s *restrict r, web_transport_poll_http_item_s *restrict item)
{
	if (item->p_last)
	{
		if ((*item->p_last = item->next))
			item->next->p_last = item->p_last;
		item->p_last = NULL;
	}
	if (item->tp)
	{
		refer_free(item->tp);
		item->tp = NULL;
	}
	if (item->http)
	{
		refer_free(item->http);
		item->http = NULL;
	}
	item->next = r->free;
	r->free = item;
}

static web_transport_poll_http_item_s* web_transport_poll_http_check_item(web_transport_poll_http_item_s *restrict item, transport_s **restrict ptp, uhttp_s **restrict phttp, uint64_t timestamp_curr, uintptr_t http_limit_size)
{
	uintptr_t n, block;
	if (timestamp_curr >= item->timestamp_dead)
		goto label_fail;
	do {
		n = 0;
		if (item->context.size < item->cache.size || exbuffer_need(&item->cache, item->cache.size << 1))
		{
			block = item->cache.size;
			if (block > http_limit_size)
				block = http_limit_size;
			if (!transport_recv(item->tp, item->cache.data, block - item->context.size, &n, NULL))
				goto label_fail;
			else if (n)
			{
				uhttp_parse_context_set(&item->context, item->cache.data, item->context.size + n);
				switch (uhttp_parse_context_try(item->http, &item->context))
				{
					case uhttp_parse_status_okay:
						if (item->context.size > item->context.pos)
						{
							if (!transport_push_recv(item->tp, item->cache.data + item->context.pos, item->context.size - item->context.pos))
								goto label_fail;
						}
						goto label_okay;
					case uhttp_parse_status_wait_header:
					case uhttp_parse_status_wait_line:
						break;
					default:
						goto label_fail;
				}
			}
		}
	} while (n && item->context.size == item->cache.size);
	if (item->context.size >= http_limit_size)
		goto label_fail;
	return NULL;
	label_okay:
	*phttp = item->http;
	item->http = NULL;
	label_fail:
	*ptp = item->tp;
	item->tp = NULL;
	return item;
}

static void web_transport_poll_http_free_func(web_transport_poll_http_s *restrict r)
{
	web_transport_poll_http_item_s *v;
	if (r->http_inst)
		refer_free(r->http_inst);
	while ((v = r->used))
	{
		r->used = v->next;
		refer_free(v);
	}
	while ((v = r->free))
	{
		r->free = v->next;
		refer_free(v);
	}
}

web_transport_poll_http_s* web_transport_poll_http_alloc(const uhttp_inst_s *http_inst, uintptr_t http_limit_size)
{
	web_transport_poll_http_s *restrict r;
	r = NULL;
	if (http_inst && http_limit_size && (r = (web_transport_poll_http_s *) refer_alloz(sizeof(web_transport_poll_http_s))))
	{
		refer_set_free(r, (refer_free_f) web_transport_poll_http_free_func);
		r->http_inst = (const uhttp_inst_s *) refer_save(http_inst);
		r->http_limit_size = http_limit_size;
	}
	return r;
}

web_transport_poll_http_s* web_transport_poll_http_join(web_transport_poll_http_s *restrict tpoll, transport_s *restrict tp, uintptr_t timeout_ms)
{
	if (tp)
	{
		web_transport_poll_http_item_s *restrict item;
		if ((item = web_transport_poll_http_get_item(tpoll, tp)))
		{
			item->timestamp_dead += timeout_ms;
			item->p_last = &tpoll->used;
			if ((item->next = tpoll->used))
				item->next->p_last = &item->next;
			tpoll->used = item;
			return tpoll;
		}
	}
	return NULL;
}

web_transport_poll_http_s* web_transport_poll_http_pop(web_transport_poll_http_s *restrict tpoll, transport_s **restrict ptp, uhttp_s **restrict phttp, uintptr_t *restrict rema_timeout_ms)
{
	web_transport_poll_http_item_s *item, *next;
	uint64_t timestamp_curr;
	if (ptp && phttp)
	{
		*ptp = NULL;
		*phttp = NULL;
		if (tpoll->used)
		{
			timestamp_curr = transport_timestamp_ms();
			item = tpoll->next_loop_first;
			do {
				if (!item)
					item = tpoll->used;
				next = item;
				if (web_transport_poll_http_check_item(item, ptp, phttp, timestamp_curr, tpoll->http_limit_size))
				{
					if (rema_timeout_ms)
					{
						if (timestamp_curr <= item->timestamp_dead)
							*rema_timeout_ms = (uintptr_t) (item->timestamp_dead - timestamp_curr);
						else *rema_timeout_ms = 0;
					}
					web_transport_poll_http_put_item(tpoll, item);
					tpoll->next_loop_first = next;
					return tpoll;
				}
			} while ((item = next) != tpoll->next_loop_first);
		}
	}
	return NULL;
}
