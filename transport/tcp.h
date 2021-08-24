#ifndef _transport_tcp_h_
#define _transport_tcp_h_

#include "transport.h"

typedef struct transport_tcp_attr_t {
	uint32_t send_timeout_ms;
	uint32_t recv_timeout_ms;
} transport_tcp_attr_t;

const char* transport_tcp_type(void);

transport_s* transport_tcp_alloc_ipv4_connect_with_bind(const char *restrict local_ip, uint32_t local_port, const char *restrict remote_ip, uint32_t remote_port, const transport_tcp_attr_t *restrict attr);
transport_s* transport_tcp_alloc_ipv4_connect(const char *restrict remote_ip, uint32_t remote_port, const transport_tcp_attr_t *restrict attr);
transport_s* transport_tcp_wait_connect(transport_s *restrict r, uint64_t connect_timeout_ms, const volatile uintptr_t *running);

#endif
