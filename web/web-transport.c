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
