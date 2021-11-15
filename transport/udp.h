#ifndef _transport_udp_h_
#define _transport_udp_h_

#include "transport.h"

const char* transport_udp_type(void);

refer_string_t transport_udp_local(transport_s *restrict r, uint32_t *restrict local_port);
const char* transport_udp_last_remote(transport_s *restrict r, uint32_t *restrict remote_port);
transport_s* transport_udp_set_remote(transport_s *restrict r, const char *restrict remote_ip, uint32_t remote_port);
transport_s* transport_udp_set_remote_by_last(transport_s *restrict r);

transport_s* transport_udp_alloc_ipv4_with_bind(const char *restrict local_ip, uint32_t local_port);
transport_s* transport_udp_alloc_ipv4(void);

#endif
