#ifndef _transport_inner_socket_h_
#define _transport_inner_socket_h_

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef int transport_inner_socket_t;

transport_inner_socket_t transport_inner_socket_create_tcp(void);

void transport_inner_socket_close(transport_inner_socket_t sock);

int transport_inner_socket_bind(transport_inner_socket_t sock, const struct sockaddr *restrict addr, socklen_t addr_size);

int transport_inner_socket_connect_start(transport_inner_socket_t sock, const struct sockaddr *restrict addr, socklen_t addr_size);
int transport_inner_socket_connect_wait(transport_inner_socket_t sock, uint64_t timeout_ms, const volatile uintptr_t *running);

int transport_inner_socket_set_timeout_send(transport_inner_socket_t sock, uint64_t timeout_ms);
int transport_inner_socket_set_timeout_recv(transport_inner_socket_t sock, uint64_t timeout_ms);
int transport_inner_socket_set_reuse_addr(transport_inner_socket_t sock);

// addr

struct sockaddr* transport_inner_socket_ipv4_addr(struct sockaddr_in *restrict addr, socklen_t *restrict len, const char *restrict ip, uint32_t port);

#endif
