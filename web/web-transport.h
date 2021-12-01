#ifndef _web_transport_h_
#define _web_transport_h_

#include <refer.h>
#include <uhttp.h>
#include <exbuffer.h>
#include <transport/transport.h>

typedef struct web_transport_attr_t {
	const volatile uintptr_t *running;
	uintptr_t http_transport_timeout_ms;
	uintptr_t http_head_max_length;
	uintptr_t http_body_max_length;
} web_transport_attr_t;

typedef struct web_transport_context_t {
	exbuffer_t *cache;
	uhttp_s *request_http;
	const void *request_body;
	uintptr_t request_body_length;
	uhttp_s *response_http;
	exbuffer_t *response_body;
} web_transport_context_t;

transport_s* web_transport_send_http(transport_s *restrict tp, const uhttp_s *restrict http, exbuffer_t *restrict cache, const web_transport_attr_t *restrict attr);
transport_s* web_transport_send_http_with_body(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict cache, const void *body, uintptr_t body_length, const web_transport_attr_t *restrict attr);
transport_s* web_transport_recv_http(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict cache, const web_transport_attr_t *restrict attr);
transport_s* web_transport_recv_body(transport_s *restrict tp, const uhttp_s *restrict http, exbuffer_t *restrict body, const web_transport_attr_t *restrict attr);
transport_s* web_transport_recv_http_with_body(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *cache, exbuffer_t *body, const web_transport_attr_t *restrict attr);
transport_s* web_transport_request(transport_s *restrict tp, const web_transport_context_t *restrict context, const web_transport_attr_t *restrict attr);

#endif
