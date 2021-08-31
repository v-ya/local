#ifndef _transport_inner_ip_h_
#define _transport_inner_ip_h_

#include <stdint.h>

#define transport_inner_port_limit  65536

uint8_t* transport_inner_ipv4_s2a(const char *restrict s_ipv4, uint8_t a_ipv4[4], uint32_t *restrict port);

#endif
