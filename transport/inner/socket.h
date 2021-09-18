#ifndef _transport_inner_socket_h_
#define _transport_inner_socket_h_

#include <refer.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

typedef int transport_inner_socket_t;

typedef union transport_inner_addr_param_t {
	struct sockaddr     addr;
	struct sockaddr_in  ipv4;
	struct sockaddr_in6 ipv6;
} transport_inner_addr_param_t;

transport_inner_socket_t transport_inner_socket_create_tcp(void);

void transport_inner_socket_close(transport_inner_socket_t sock);

int transport_inner_socket_bind(transport_inner_socket_t sock, const struct sockaddr *restrict addr, socklen_t addr_size);

int transport_inner_socket_connect_start(transport_inner_socket_t sock, const struct sockaddr *restrict addr, socklen_t addr_size);
int transport_inner_socket_connect_wait(transport_inner_socket_t sock, uint64_t timeout_ms, const volatile uintptr_t *running);

int transport_inner_socket_listen(transport_inner_socket_t sock, int n);
int transport_inner_socket_accept_wait(transport_inner_socket_t sock, transport_inner_socket_t *restrict rsock, uint64_t timeout_ms, const volatile uintptr_t *running);

int transport_inner_socket_set_timeout_send(transport_inner_socket_t sock, uint64_t timeout_ms);
int transport_inner_socket_set_timeout_recv(transport_inner_socket_t sock, uint64_t timeout_ms);
int transport_inner_socket_set_reuse_addr(transport_inner_socket_t sock);

struct sockaddr* transport_inner_socket_get_local(transport_inner_socket_t sock, transport_inner_addr_param_t *restrict addr);
struct sockaddr* transport_inner_socket_get_remote(transport_inner_socket_t sock, transport_inner_addr_param_t *restrict addr);

// addr

struct sockaddr* transport_inner_socket_ipv4_addr(struct sockaddr_in *restrict addr, socklen_t *restrict len, const char *restrict ip, uint32_t port);

refer_string_t transport_inner_socket_addr2info(struct sockaddr *restrict addr, uint32_t *restrict port);

#endif
