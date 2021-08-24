#include "tcp.h"
#include "inner/socket.h"

typedef enum transport_tcp_status_t {
	transport_tcp_status_okay,
	transport_tcp_status_connect_start,
} transport_tcp_status_t;

typedef struct transport_tcp_s {
	transport_s tp;
	transport_inner_socket_t sock;
	transport_tcp_status_t status;
	refer_string_t local_ip;
	refer_string_t remote_ip;
	uint32_t local_port;
	uint32_t remote_port;
} transport_tcp_s;

static const char *t_type = "TCP";

#define me(_r)  ((transport_tcp_s *) _r)

const char* transport_tcp_type(void)
{
	return t_type;
}

static void transport_tcp_free_func(transport_tcp_s *restrict r)
{
	if (~r->sock)
		transport_inner_socket_close(r->sock);
	if (r->local_ip)
		refer_free(r->local_ip);
	if (r->remote_ip)
		refer_free(r->remote_ip);
}

static transport_s* transport_tcp_send(transport_tcp_s *restrict r, const void *data, uintptr_t n, uintptr_t *restrict rn)
{
	*rn = 0;
	if (r->status == transport_tcp_status_okay)
	{
		ssize_t ret;
		if ((ret = send(r->sock, data, n, MSG_DONTWAIT)) >= 0)
		{
			*rn = (uintptr_t) ret;
			return &r->tp;
		}
	}
	return NULL;
}

static transport_s* transport_tcp_recv(transport_tcp_s *restrict r, void *data, uintptr_t n, uintptr_t *restrict rn)
{
	*rn = 0;
	if (r->status == transport_tcp_status_okay)
	{
		ssize_t ret;
		if ((ret = recv(r->sock, data, n, MSG_DONTWAIT)) >= 0)
		{
			*rn = (uintptr_t) ret;
			label_return:
			return &r->tp;
		}
		else if (errno == EAGAIN)
			goto label_return;
	}
	return NULL;
}

transport_s* transport_tcp_alloc_ipv4_connect_with_bind(const char *restrict local_ip, uint32_t local_port, const char *restrict remote_ip, uint32_t remote_port, const transport_tcp_attr_t *restrict attr)
{
	transport_tcp_s *restrict r;
	struct sockaddr_in remote, local;
	struct sockaddr *remote_addr, *local_addr;
	socklen_t remote_len, local_len;
	if ((remote_addr = transport_inner_socket_ipv4_addr(&remote, &remote_len, remote_ip, remote_port)))
	{
		r = (transport_tcp_s *) refer_alloz(sizeof(transport_tcp_s));
		if (r)
		{
			r->sock = (transport_inner_socket_t) -1;
			r->status = transport_tcp_status_connect_start;
			transport_initial(&r->tp, t_type, NULL, (transport_send_f) transport_tcp_send, (transport_recv_f) transport_tcp_recv);
			refer_set_free(r, (refer_free_f) transport_tcp_free_func);
			r->sock = transport_inner_socket_create_tcp();
			if (~r->sock)
			{
				if (local_ip && local_port)
				{
					if (!(local_addr = transport_inner_socket_ipv4_addr(&local, &local_len, local_ip, local_port)))
						goto label_fail;
					if (transport_inner_socket_set_reuse_addr(r->sock))
						goto label_fail;
					if (transport_inner_socket_bind(r->sock, local_addr, local_len))
						goto label_fail;
				}
				if (attr)
				{
					if (transport_inner_socket_set_timeout_send(r->sock, (uint64_t) attr->send_timeout_ms))
						goto label_fail;
					if (transport_inner_socket_set_timeout_recv(r->sock, (uint64_t) attr->recv_timeout_ms))
						goto label_fail;
				}
				if (!transport_inner_socket_connect_start(r->sock, remote_addr, remote_len))
					return &r->tp;
			}
			label_fail:
			refer_free(r);
		}
	}
	return NULL;
}

transport_s* transport_tcp_alloc_ipv4_connect(const char *restrict remote_ip, uint32_t remote_port, const transport_tcp_attr_t *restrict attr)
{
	return transport_tcp_alloc_ipv4_connect_with_bind(NULL, 0, remote_ip, remote_port, attr);
}

transport_s* transport_tcp_wait_connect(transport_s *restrict r, uint64_t connect_timeout_ms, const volatile uintptr_t *running)
{
	if (r->type == t_type && me(r)->status == transport_tcp_status_connect_start)
	{
		if (!transport_inner_socket_connect_wait(me(r)->sock, connect_timeout_ms, running))
		{
			me(r)->status = transport_tcp_status_okay;
			return r;
		}
	}
	return NULL;
}
