#include "socket.h"
#include "time.h"
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <memory.h>

transport_inner_socket_t transport_inner_socket_create_tcp(void)
{
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

void transport_inner_socket_close(transport_inner_socket_t sock)
{
	close(sock);
}

int transport_inner_socket_bind(transport_inner_socket_t sock, const struct sockaddr *restrict addr, socklen_t addr_size)
{
	if (!bind(sock, addr, addr_size))
		return 0;
	return -1;
}

int transport_inner_socket_connect_start(transport_inner_socket_t sock, const struct sockaddr *restrict addr, socklen_t addr_size)
{
	int flags, en;
	if (~(flags = fcntl(sock, F_GETFL)) && ~fcntl(sock, F_SETFL, flags | O_NONBLOCK))
	{
		if (!connect(sock, addr, addr_size) || (en = errno) == EAGAIN || en == EINPROGRESS)
			return 0;
	}
	return -1;
}

int transport_inner_socket_connect_wait(transport_inner_socket_t sock, uint64_t timeout_ms)
{
	struct timespec start, curr;
	uint64_t cost;
	if (transport_inner_timespec_now(&start))
	{
		struct pollfd pfd;
		int value;
		socklen_t len;
		pfd.fd = sock;
		pfd.events = POLLOUT;
		pfd.revents = 0;
		cost = 0;
		do {
			value = poll(&pfd, 1, timeout_ms - cost);
			if (value > 0)
			{
				len = sizeof(value);
				if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &value, &len) < 0)
					break;
				if (!value && ~(value = fcntl(sock, F_GETFL)) && ~fcntl(sock, F_SETFL, value & ~O_NONBLOCK))
					return 0;
				break;
			}
			if (!transport_inner_timespec_now(&curr))
				break;
			cost = transport_inner_timespec_to_ms(transport_inner_timespec_diff(&curr, &curr, &start));
		} while (cost < timeout_ms);
	}
	return -1;
}

int transport_inner_socket_set_timeout_send(transport_inner_socket_t sock, uint64_t timeout_ms)
{
	struct timeval timeout;
	transport_inner_timeval_by_ms(&timeout, timeout_ms);
	return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

int transport_inner_socket_set_timeout_recv(transport_inner_socket_t sock, uint64_t timeout_ms)
{
	struct timeval timeout;
	transport_inner_timeval_by_ms(&timeout, timeout_ms);
	return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

int transport_inner_socket_set_reuse_addr(transport_inner_socket_t sock)
{
	int value = 1;
	return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
}

// addr

#include "ip.h"

struct sockaddr* transport_inner_socket_ipv4_addr(struct sockaddr_in *restrict addr, socklen_t *restrict len, const char *restrict ip, uint32_t port)
{
	uint8_t ipv4[4];
	if (len) *len = sizeof(struct sockaddr_in);
	if (ip && port && port < 65536 && transport_inner_ipv4_s2a(ip, ipv4))
	{
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = htonl(((((((uint32_t) ipv4[0] << 8) | ipv4[1]) << 8) | ipv4[2]) << 8) | ipv4[3]);
		addr->sin_port = htons((uint16_t) port);
		memset(addr->sin_zero, 0, sizeof(addr->sin_zero));
		return (struct sockaddr *) addr;
	}
	return NULL;
}

