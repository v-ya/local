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
	transport_final(&r->tp);
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

static transport_tcp_s* transport_tcp_alloc_empty(void)
{
	transport_tcp_s *restrict r;
	r = (transport_tcp_s *) refer_alloz(sizeof(transport_tcp_s));
	if (r)
	{
		r->sock = (transport_inner_socket_t) -1;
		r->status = transport_tcp_status_connect_start;
		refer_set_free(r, (refer_free_f) transport_tcp_free_func);
		transport_initial(&r->tp, t_type, NULL, (transport_send_f) transport_tcp_send, (transport_recv_f) transport_tcp_recv);
	}
	return r;
}

refer_string_t transport_tcp_local(transport_s *restrict r, uint32_t *restrict local_port)
{
	transport_inner_addr_param_t param;
	struct sockaddr *restrict addr;
	if (local_port)
		*local_port = 0;
	if (r->type == t_type)
	{
		if (me(r)->local_ip)
		{
			label_okay:
			if (local_port)
				*local_port = me(r)->local_port;
			return me(r)->local_ip;
		}
		if ((addr = transport_inner_socket_get_local(me(r)->sock, &param)) &&
			(me(r)->local_ip = transport_inner_socket_addr2info(addr, &me(r)->local_port)))
			goto label_okay;
	}
	return NULL;
}

refer_string_t transport_tcp_remote(transport_s *restrict r, uint32_t *restrict remote_port)
{
	transport_inner_addr_param_t param;
	struct sockaddr *restrict addr;
	if (remote_port)
		*remote_port = 0;
	if (r->type == t_type)
	{
		if (me(r)->remote_ip)
		{
			label_okay:
			if (remote_port)
				*remote_port = me(r)->remote_port;
			return me(r)->remote_ip;
		}
		if ((addr = transport_inner_socket_get_remote(me(r)->sock, &param)) &&
			(me(r)->remote_ip = transport_inner_socket_addr2info(addr, &me(r)->remote_port)))
			goto label_okay;
	}
	return NULL;
}

transport_s* transport_tcp_alloc_ipv4_connect_with_bind(const char *restrict local_ip, uint32_t local_port, const char *restrict remote_ip, uint32_t remote_port)
{
	transport_tcp_s *restrict r;
	struct sockaddr_in remote, local;
	struct sockaddr *remote_addr, *local_addr;
	socklen_t remote_len, local_len;
	if ((remote_addr = transport_inner_socket_ipv4_addr(&remote, &remote_len, remote_ip, remote_port)))
	{
		if ((r = transport_tcp_alloc_empty()))
		{
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
				if (transport_inner_socket_set_nonblock(r->sock))
					goto label_fail;
				if (!transport_inner_socket_connect_start(r->sock, remote_addr, remote_len))
					return &r->tp;
			}
			label_fail:
			refer_free(r);
		}
	}
	return NULL;
}

transport_s* transport_tcp_alloc_ipv4_connect(const char *restrict remote_ip, uint32_t remote_port)
{
	return transport_tcp_alloc_ipv4_connect_with_bind(NULL, 0, remote_ip, remote_port);
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

// tcp server

struct transport_tcp_server_s {
	transport_inner_socket_t sock;
};

static void transport_tcp_server_free_func(transport_tcp_server_s *restrict r)
{
	if (~r->sock)
		transport_inner_socket_close(r->sock);
}

transport_tcp_server_s* transport_tcp_server_alloc(const char *restrict local_ip, uint32_t local_port, uint32_t nlisten)
{
	transport_tcp_server_s *restrict r;
	struct sockaddr_in local;
	struct sockaddr *local_addr;
	socklen_t local_len;
	if (nlisten && nlisten <= 0x7fffffff &&
		(local_addr = transport_inner_socket_ipv4_addr(&local, &local_len, local_ip, local_port)))
	{
		r = (transport_tcp_server_s *) refer_alloz(sizeof(transport_tcp_server_s));
		if (r)
		{
			r->sock = (transport_inner_socket_t) -1;
			refer_set_free(r, (refer_free_f) transport_tcp_server_free_func);
			r->sock = transport_inner_socket_create_tcp();
			if (~r->sock)
			{
				if (transport_inner_socket_set_reuse_addr(r->sock))
					goto label_fail;
				if (transport_inner_socket_bind(r->sock, local_addr, local_len))
					goto label_fail;
				if (transport_inner_socket_listen(r->sock, (int) nlisten))
					goto label_fail;
				return r;
			}
			label_fail:
			refer_free(r);
		}
	}
	return NULL;
}

transport_tcp_server_s* transport_tcp_server_accept(transport_tcp_server_s *restrict r, transport_s **restrict ptp, uint64_t accept_timeout_ms, const volatile uintptr_t *running)
{
	transport_tcp_s *restrict tp;
	transport_inner_socket_t rsock;
	if (ptp) *ptp = NULL;
	if (!transport_inner_socket_accept_wait(r->sock, &rsock, accept_timeout_ms, running))
	{
		if (!~rsock)
			goto label_okay;
		if (~rsock)
		{
			if (ptp)
			{
				if (!(tp = transport_tcp_alloc_empty()))
					goto label_fail;
				tp->sock = rsock;
				tp->status = transport_tcp_status_okay;
				*ptp = &tp->tp;
			}
			else transport_inner_socket_close(rsock);
			label_okay:
			return r;
		}
	}
	label_fail:
	if (~rsock)
		transport_inner_socket_close(rsock);
	return NULL;
}
