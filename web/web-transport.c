#include "web-transport.h"

#define web_transport_define_timeout_ms  5000

static const char *s_http_header_id_context_length = "context-length";

transport_s* web_transport_send_http(transport_s *restrict tp, const uhttp_s *restrict http, exbuffer_t *restrict cache)
{
	char *p;
	uintptr_t n, rn;
	if ((p = uhttp_build(http, cache, &n)))
	{
		if (transport_send(tp, p, n, &rn) && rn == n)
			return tp;
	}
	return NULL;
}

transport_s* web_transport_send_http_with_body(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict cache, const char *body, uintptr_t body_length)
{
	uintptr_t rn;
	if (uhttp_find_header_first(http, s_http_header_id_context_length) ||
		uhttp_insert_header_integer(http, "Context-Length", (int64_t) body_length))
	{
		if (web_transport_send_http(tp, http, cache))
		{
			if (transport_send(tp, body, body_length, &rn) && rn == body_length)
				return tp;
		}
	}
	return NULL;
}

transport_s* web_transport_recv_http(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict cache, const web_transport_recv_attr_t *restrict attr, uintptr_t *restrict cost_time_ms)
{
	uhttp_parse_context_t context;
	transport_recv_attr_t ra;
	uint64_t start, die, now;
	uintptr_t rn, block;
	void *data;
	start = now = transport_timestamp_ms();
	die = start + (attr?attr->http_recv_timeout_ms:web_transport_define_timeout_ms);
	uhttp_parse_context_init(&context, 0);
	ra.running = attr?attr->running:NULL;
	ra.recv_some = 1;
	ra.recv_full = 0;
	block = 4096;
	if (attr && block > attr->http_head_max_length)
		block = attr->http_head_max_length;
	if ((data = exbuffer_need(cache, block)))
	{
		uhttp_parse_context_set(&context, data, 0);
		do {
			ra.timeout_ms = die - now;
			if (!transport_recv(tp, cache, block - context.size, &rn, &ra))
				break;
			if (rn)
			{
				context.size += rn;
				switch (uhttp_parse_context_try(http, &context))
				{
					case uhttp_parse_status_okay:
						if (context.size > context.pos)
							tp = transport_push_recv(tp, data + context.pos, context.size - context.pos);
						if (cost_time_ms)
							*cost_time_ms = transport_timestamp_ms() - start;
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
		} while ((now = transport_timestamp_ms()) < die);
	}
	label_fail:
	if (cost_time_ms)
		*cost_time_ms = transport_timestamp_ms() - start;
	return NULL;
}

transport_s* web_transport_recv_body(transport_s *restrict tp, const uhttp_s *restrict http, exbuffer_t *restrict body, const web_transport_recv_attr_t *restrict attr, uintptr_t *restrict cost_time_ms)
{
	transport_recv_attr_t ra;
	uint64_t start, now;
	uintptr_t body_length;
	void *data;
	start = now = 0;
	body->used = 0;
	if ((body_length = (uintptr_t) uhttp_get_header_integer_first(http, s_http_header_id_context_length)))
	{
		if (attr && body_length > attr->http_body_max_length)
			goto label_fail;
		if (!(data = exbuffer_need(body, body_length)))
			goto label_fail;
		start = transport_timestamp_ms();
		ra.timeout_ms = attr?attr->http_recv_timeout_ms:web_transport_define_timeout_ms;
		ra.running = attr?attr->running:NULL;
		ra.recv_some = 0;
		ra.recv_full = 1;
		tp = transport_recv(tp, data, body_length, &body_length, &ra);
		body->used = body_length;
		now = transport_timestamp_ms();
	}
	label_quit:
	if (cost_time_ms)
		*cost_time_ms = now - start;
	return tp;
	label_fail:
	tp = NULL;
	goto label_quit;
}

transport_s* web_transport_recv_http_with_body(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict body, const web_transport_recv_attr_t *restrict attr, uintptr_t *restrict cost_time_ms)
{
	web_transport_recv_attr_t a;
	uintptr_t cost1, cost2;
	cost1 = cost2 = 0;
	body->used = 0;
	if (attr) a = *attr, attr = &a;
	tp = web_transport_recv_http(tp, http, body, attr, &cost1);
	if (tp)
	{
		if (attr) a.http_recv_timeout_ms = (a.http_recv_timeout_ms > cost1)?(a.http_recv_timeout_ms - cost1):0;
		tp = web_transport_recv_body(tp, http, body, attr, &cost2);
	}
	if (cost_time_ms)
		*cost_time_ms = cost1 + cost2;
	return tp;
}
