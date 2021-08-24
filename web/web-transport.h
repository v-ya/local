#ifndef _web_transport_h_
#define _web_transport_h_

#include <refer.h>
#include <uhttp.h>
#include <transport/tcp.h>

typedef struct web_transport_recv_attr_t {
	const volatile uintptr_t *running;
	uintptr_t http_head_max_length;
	uintptr_t http_body_max_length;
	uintptr_t http_recv_timeout_ms;
} web_transport_recv_attr_t;

transport_s* web_transport_send_http(transport_s *restrict tp, const uhttp_s *restrict http, exbuffer_t *restrict cache);
transport_s* web_transport_send_http_with_body(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict cache, const char *body, uintptr_t body_length);
transport_s* web_transport_recv_http(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict cache, const web_transport_recv_attr_t *restrict attr, uintptr_t *restrict cost_time_ms);
transport_s* web_transport_recv_body(transport_s *restrict tp, const uhttp_s *restrict http, exbuffer_t *restrict body, const web_transport_recv_attr_t *restrict attr, uintptr_t *restrict cost_time_ms);
transport_s* web_transport_recv_http_with_body(transport_s *restrict tp, uhttp_s *restrict http, exbuffer_t *restrict body, const web_transport_recv_attr_t *restrict attr, uintptr_t *restrict cost_time_ms);

#endif
