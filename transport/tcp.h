#ifndef _transport_tcp_h_
#define _transport_tcp_h_

#include "transport.h"

typedef struct transport_tcp_server_s transport_tcp_server_s;

const char* transport_tcp_type(void);

refer_string_t transport_tcp_local(transport_s *restrict r, uint32_t *restrict local_port);
refer_string_t transport_tcp_remote(transport_s *restrict r, uint32_t *restrict remote_port);

transport_s* transport_tcp_alloc_ipv4_connect_with_bind(const char *restrict local_ip, uint32_t local_port, const char *restrict remote_ip, uint32_t remote_port);
transport_s* transport_tcp_alloc_ipv4_connect(const char *restrict remote_ip, uint32_t remote_port);
transport_s* transport_tcp_wait_connect(transport_s *restrict r, uint64_t connect_timeout_ms, const volatile uintptr_t *running);

transport_tcp_server_s* transport_tcp_server_alloc(const char *restrict local_ip, uint32_t local_port, uint32_t nlisten);
transport_tcp_server_s* transport_tcp_server_accept(transport_tcp_server_s *restrict r, transport_s **restrict ptp, uint64_t accept_timeout_ms, const volatile uintptr_t *running);

#endif
