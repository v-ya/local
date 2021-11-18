#include "udp.h"
#include "inner/socket.h"

typedef struct transport_udp_s {
	transport_s tp;
	transport_inner_socket_t sock;
	socklen_t addr_length;
	transport_inner_addr_param_t remote_addr;
	transport_inner_addr_param_t last_remote_addr;
	refer_string_t local_ip;
	char *last_remote_ip;
	uint32_t local_port;
	uint32_t last_remote_port;
	char ip_buffer[40];
} transport_udp_s;

static const char *t_type = "UDP";

#define me(_r)  ((transport_udp_s *) _r)

const char* transport_udp_type(void)
{
	return t_type;
}

static void transport_udp_free_func(transport_udp_s *restrict r)
{
	if (~r->sock)
		transport_inner_socket_close(r->sock);
	if (r->local_ip)
		refer_free(r->local_ip);
	transport_final(&r->tp);
}

static transport_s* transport_udp_send(transport_udp_s *restrict r, const void *data, uintptr_t n, uintptr_t *restrict rn)
{
	ssize_t ret;
	*rn = 0;
	if ((ret = sendto(r->sock, data, n, MSG_DONTWAIT, &r->remote_addr.addr, r->addr_length)) >= 0)
	{
		*rn = (uintptr_t) ret;
		label_return:
		return &r->tp;
	}
	else if (errno == EAGAIN)
		goto label_return;
	return NULL;
}

static transport_s* transport_udp_recv(transport_udp_s *restrict r, void *data, uintptr_t n, uintptr_t *restrict rn)
{
	ssize_t ret;
	socklen_t length;
	*rn = 0;
	length = r->addr_length;
	if ((ret = recvfrom(r->sock, data, n, MSG_DONTWAIT, &r->last_remote_addr.addr, &length)) >= 0)
	{
		*rn = (uintptr_t) ret;
		label_return:
		return &r->tp;
	}
	else if (errno == EAGAIN)
		goto label_return;
	return NULL;
}

static transport_udp_s* transport_udp_alloc_empty(socklen_t addr_length)
{
	transport_udp_s *restrict r;
	r = (transport_udp_s *) refer_alloz(sizeof(transport_udp_s));
	if (r)
	{
		r->sock = (transport_inner_socket_t) -1;
		r->addr_length = addr_length;
		r->last_remote_ip = r->ip_buffer;
		refer_set_free(r, (refer_free_f) transport_udp_free_func);
		transport_initial(&r->tp, t_type, NULL, (transport_send_f) transport_udp_send, (transport_recv_f) transport_udp_recv);
	}
	return r;
}

refer_string_t transport_udp_local(transport_s *restrict r, uint32_t *restrict local_port)
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

const char* transport_udp_last_remote(transport_s *restrict r, uint32_t *restrict remote_port)
{
	if (remote_port)
		*remote_port = 0;
	if (r->type == t_type)
		return transport_inner_socket_addr2info_write(me(r)->last_remote_ip, &me(r)->last_remote_addr.addr, &me(r)->last_remote_port);
	return NULL;
}

transport_s* transport_udp_set_remote(transport_s *restrict r, const char *restrict remote_ip, uint32_t remote_port)
{
	if (r->type == t_type && remote_ip && remote_port)
	{
		struct sockaddr *addr;
		if (me(r)->addr_length == sizeof(struct sockaddr_in))
			addr = transport_inner_socket_ipv4_addr(&me(r)->remote_addr.ipv4, NULL, remote_ip, remote_port);
		else if (me(r)->addr_length == sizeof(struct sockaddr_in6))
			addr = NULL;
		else addr = NULL;
		if (addr)
			return r;
	}
	return NULL;
}

transport_s* transport_udp_set_remote_by_last(transport_s *restrict r)
{
	if (r->type == t_type)
	{
		me(r)->remote_addr = me(r)->last_remote_addr;
		return r;
	}
	return NULL;
}

transport_s* transport_udp_alloc_ipv4_with_bind(const char *restrict local_ip, uint32_t local_port)
{
	transport_udp_s *restrict r;
	struct sockaddr_in local;
	struct sockaddr *local_addr;
	socklen_t local_len;
	if ((r = transport_udp_alloc_empty(sizeof(struct sockaddr_in))))
	{
		r->sock = transport_inner_socket_create_udp();
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
			return &r->tp;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

transport_s* transport_udp_alloc_ipv4(void)
{
	return transport_udp_alloc_ipv4_with_bind(NULL, 0);
}
