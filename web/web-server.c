#include "web-server.h"
#include "web-transport.h"
#include "web-header.h"
#include <transport/tcp.h>
#include <queue/queue.h>
#include <hashmap.h>
#include <yaw.h>
#include <string.h>

#define web_server_time_gap_msec  50

typedef struct web_server_bind_s web_server_bind_s;
typedef struct web_server_pri_s web_server_pri_s;
typedef struct web_server_request_s web_server_request_s;

struct web_server_route_s {
	web_server_request_f func;
	refer_t pri;
	web_server_request_flag_t flags;
};

struct web_server_bind_s {
	web_server_bind_s *next;
	web_server_pri_s *weak_inst;
	transport_tcp_server_s *listen;
	yaw_s *thread;
};

struct web_server_pri_s {
	web_server_s server;
	web_server_status_t status;
	web_server_limit_t limit;
	yaw_lock_s *register_read;
	yaw_lock_s *register_write;
	yaw_lock_s *detach;
	yaw_signal_s *signal;
	web_server_bind_s *bind_list;
	queue_s *q_transport;
	queue_s *q_detach;
	const web_server_route_s *pretreat_route;
	hashmap_t request_route;  // method => uri => (web_server_route_t *)
	hashmap_t response_route; // code => (web_server_route_t *)
	yaw_s *working_array[];
};

struct web_server_request_s {
	web_server_request_t request;
	exbuffer_t request_body;
	exbuffer_t response_body;
	const web_server_route_s *route;
	uint64_t recv_timeout_ms;
};

static const char *s_http_header_id_content_length = "content-length";
static const char *s_http_header_id_connection     = "connection";

static void web_server_thread_bind(yaw_s *restrict yaw);
static void web_server_thread_working(yaw_s *restrict yaw);
static void web_server_thread_detach(yaw_s *restrict yaw);

static void web_server_detach_clear(queue_s *q_detach)
{
	yaw_s *y;
	queue_pull_f pull;
	pull = q_detach->pull;
	while ((y = (yaw_s *) pull(q_detach)))
	{
		yaw_stop_and_wait(y);
		refer_free(y);
	}
}

static void web_server_route_free_func(web_server_route_s *restrict r)
{
	if (r->pri)
		refer_free(r->pri);
}

static void web_server_bind_free_func(web_server_bind_s *restrict r)
{
	if (r->thread)
	{
		yaw_stop_and_wait(r->thread);
		refer_free(r->thread);
	}
	if (r->listen)
		refer_free(r->listen);
}

static web_server_bind_s* web_server_bind_alloc(web_server_pri_s *restrict inst, const char *restrict ip, uint32_t port, uint32_t nlisten)
{
	web_server_bind_s *restrict r;
	r = (web_server_bind_s *) refer_alloz(sizeof(web_server_bind_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) web_server_bind_free_func);
		r->weak_inst = inst;
		r->listen = transport_tcp_server_alloc(ip, port, nlisten);
		if (r->listen)
		{
			r->thread = yaw_alloc_and_start(web_server_thread_bind, NULL, r);
			if (r->thread)
				return r;
		}
		refer_free(r);
	}
	return NULL;
}

static void web_server_pri_clear_bind_list(web_server_bind_s *list)
{
	web_server_bind_s *p;
	while ((p = list))
	{
		list = p->next;
		refer_free(p);
	}
}

static void web_server_request_free_func(web_server_request_s *restrict r)
{
	if (r->request.tp)
		refer_free(r->request.tp);
	if (r->request.request_uri)
		refer_free(r->request.request_uri);
	if (r->request.request_http)
		refer_free(r->request.request_http);
	if (r->request.response_http)
		refer_free(r->request.response_http);
	if (r->route)
		refer_free(r->route);
	exbuffer_uini(&r->request_body);
	exbuffer_uini(&r->response_body);
}

static web_server_request_s* web_server_request_alloc(const web_server_s *server, transport_s *tp, uhttp_s *request_http)
{
	web_server_request_s *restrict r;
	if ((r = (web_server_request_s *) refer_alloz(sizeof(web_server_request_s))))
	{
		refer_set_free(r, (refer_free_f) web_server_request_free_func);
		if (exbuffer_init(&r->request_body, 0) &&
			exbuffer_init(&r->response_body, 0) &&
			(r->request.request_uri = uhttp_uri_alloc()) &&
			(r->request.response_http = uhttp_alloc(server->http_inst)))
		{
			r->request.server = server;
			r->request.tp = (transport_s *) refer_save(tp);
			r->request.request_http = (uhttp_s *) refer_save(request_http);
			r->request.request_body = &r->request_body;
			r->request.response_body = &r->response_body;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static void web_server_pri_free_func(web_server_pri_s *restrict r)
{
	uintptr_t i;
	r->server.running = 0;
	if (r->signal)
	{
		yaw_signal_inc(r->signal);
		yaw_signal_wake(r->signal, ~0);
	}
	// release working
	for (i = 0; i < r->limit.working_number; ++i)
	{
		if (r->working_array[i])
		{
			yaw_stop_and_wait(r->working_array[i]);
			refer_free(r->working_array[i]);
		}
	}
	// release detach (wait)
	while (r->status.detach)
		yaw_msleep(web_server_time_gap_msec);
	// release bind
	web_server_pri_clear_bind_list(r->bind_list);
	// release queue
	if (r->q_transport)
		refer_free(r->q_transport);
	if (r->q_detach)
	{
		web_server_detach_clear(r->q_detach);
		refer_free(r->q_detach);
	}
	// release lock && signal
	if (r->register_read)
		refer_free(r->register_read);
	if (r->register_write)
		refer_free(r->register_write);
	if (r->detach)
		refer_free(r->detach);
	if (r->signal)
		refer_free(r->signal);
	// release route
	if (r->pretreat_route)
		refer_free(r->pretreat_route);
	hashmap_uini(&r->request_route);
	hashmap_uini(&r->response_route);
	// release server
	if (r->server.http_inst)
		refer_free(r->server.http_inst);
	if (r->server.headers)
		refer_free(r->server.headers);
}

static void web_server_limit_fix(web_server_limit_t *restrict dst, const web_server_limit_t *restrict src)
{
	for (uintptr_t i = 0; i < (sizeof(web_server_limit_t) / sizeof(uintptr_t)); ++i)
		if (!((uintptr_t *) dst)[i])
			((uintptr_t *) dst)[i] = ((const uintptr_t *) src)[i];
}

web_server_s* web_server_alloc(const uhttp_inst_s *restrict http_inst, const web_server_limit_t *restrict limit)
{
	static const web_server_limit_t _limit = {
		.http_max_length = (128 << 10),     // 128 KiB
		.body_max_length = (16 << 20),      //  16 MiB
		.transport_recv_timeout_ms = 5000,  //   5 sec
		.transport_send_timeout_ms = 3000,  //   3 sec
		.working_number = 1,
		.detach_number = 1024,
		.queue_depth_size = 1024,
		.wait_req_max_number = 32,
	};
	web_server_pri_s *restrict r;
	yaw_s **working;
	uintptr_t i, n;
	if (!limit) limit = &_limit;
	n = limit->working_number;
	if (!n) n = _limit.working_number;
	if (n && (r = (web_server_pri_s *) refer_alloz(sizeof(web_server_pri_s) + n * sizeof(yaw_s *))))
	{
		refer_set_free(r, (refer_free_f) web_server_pri_free_func);
		// limit && status
		r->limit = *limit;
		web_server_limit_fix(&r->limit, &_limit);
		r->limit.working_number = n;
		if (r->limit.queue_depth_size < 16)
			r->limit.queue_depth_size = 16;
		// server
		r->server.running = 1;
		r->server.http_inst = http_inst?((const uhttp_inst_s *) refer_save(http_inst)):uhttp_inst_alloc_http11();
		r->server.headers = web_header_alloc();
		r->server.limit = &r->limit;
		r->server.status = &r->status;
		if (!r->server.http_inst)
			goto label_fail;
		if (!r->server.headers)
			goto label_fail;
		// hashmap
		if (!hashmap_init(&r->request_route))
			goto label_fail;
		if (!hashmap_init(&r->response_route))
			goto label_fail;
		// lock && signal
		if (yaw_lock_alloc_rwlock(&r->register_read, &r->register_write))
			goto label_fail;
		if (!(r->detach = yaw_lock_alloc_mutex()))
			goto label_fail;
		if (!(r->signal = yaw_signal_alloc()))
			goto label_fail;
		// queue
		if (!(r->q_transport = queue_alloc_ring(r->limit.queue_depth_size)))
			goto label_fail;
		if (!(r->q_detach = queue_alloc_ring(r->limit.queue_depth_size)))
			goto label_fail;
		// working
		working = r->working_array;
		for (i = 0; i < n; ++i)
		{
			if (!(working[i] = yaw_alloc_and_start(web_server_thread_working, NULL, r)))
				goto label_fail;
		}
		return &r->server;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

web_server_s* web_server_add_bind(web_server_s *server, const char *restrict ip, uint32_t port, uint32_t nlisten)
{
	web_server_pri_s *r;
	web_server_bind_s *restrict bind;
	r = (web_server_pri_s *) server;
	if ((bind = web_server_bind_alloc(r, ip, port, nlisten)))
	{
		yaw_lock_lock(r->register_write);
		bind->next = r->bind_list;
		r->bind_list = bind;
		yaw_lock_unlock(r->register_write);
		return server;
	}
	return NULL;
}

web_server_route_s* web_server_route_alloc(web_server_request_f func, refer_t pri, web_server_request_flag_t flags)
{
	web_server_route_s *restrict r;
	r = NULL;
	if (func && (r = (web_server_route_s *) refer_alloc(sizeof(web_server_route_s))))
	{
		r->pri = NULL;
		refer_set_free(r, (refer_free_f) web_server_route_free_func);
		r->func = func;
		r->pri = refer_save(pri);
		r->flags = flags;
	}
	return r;
}

web_server_s* web_server_register_pretreat_by_route(web_server_s *server, const web_server_route_s *restrict route)
{
	web_server_pri_s *restrict p;
	p = (web_server_pri_s *) server;
	yaw_lock_lock(p->register_write);
	if (p->pretreat_route)
		refer_free(p->pretreat_route);
	p->pretreat_route = (const web_server_route_s *) refer_save(route);
	yaw_lock_unlock(p->register_write);
	return server;
}

static void web_server_inner_request_route_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		hashmap_free((hashmap_t *) vl->value);
}

static void web_server_inner_uri_mapping_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		refer_free(vl->value);
}

static web_server_s* web_server_inner_register_request(web_server_pri_s *restrict p, const char *restrict method, const char *restrict uri, const web_server_route_s *restrict route)
{
	hashmap_t *restrict uri_mapping;
	if ((uri_mapping = hashmap_get_name(&p->request_route, method)))
	{
		label_uri_mapping:
		if (hashmap_set_name(uri_mapping, uri, route, web_server_inner_uri_mapping_free_func))
		{
			refer_save(route);
			return &p->server;
		}
	}
	else if ((uri_mapping = hashmap_alloc()))
	{
		if (hashmap_set_name(&p->request_route, method, uri_mapping, web_server_inner_request_route_free_func))
			goto label_uri_mapping;
		hashmap_free(uri_mapping);
	}
	return NULL;
}

static web_server_s* web_server_inner_register_response(web_server_pri_s *restrict p, int code, const web_server_route_s *restrict route)
{
	if (hashmap_set_head(&p->response_route, (uint64_t) code, route, web_server_inner_uri_mapping_free_func))
	{
		refer_save(route);
		return &p->server;
	}
	return NULL;
}

static const web_server_route_s* web_server_inner_find_request(web_server_pri_s *restrict p, const char *restrict method, const char *restrict uri)
{
	hashmap_t *restrict uri_mapping;
	if ((uri_mapping = hashmap_get_name(&p->request_route, method)))
		return (const web_server_route_s *) hashmap_get_name(uri_mapping, uri);
	return NULL;
}

static const web_server_route_s* web_server_inner_find_response(web_server_pri_s *restrict p, int code)
{
	return (const web_server_route_s *) hashmap_get_head(&p->response_route, (uint64_t) code);
}

web_server_s* web_server_register_request_by_route(web_server_s *server, const char *restrict method, const char *restrict uri, const web_server_route_s *restrict route)
{
	web_server_pri_s *p;
	p = (web_server_pri_s *) server;
	server = NULL;
	if (method && uri)
	{
		yaw_lock_lock(p->register_write);
		server = web_server_inner_register_request(p, method, uri, route);
		yaw_lock_unlock(p->register_write);
	}
	return server;
}

web_server_s* web_server_register_response_by_route(web_server_s *server, int code, const web_server_route_s *restrict route)
{
	web_server_pri_s *p;
	p = (web_server_pri_s *) server;
	yaw_lock_lock(p->register_write);
	server = web_server_inner_register_response(p, code, route);
	yaw_lock_unlock(p->register_write);
	return server;
}

web_server_s* web_server_register_pretreat(web_server_s *server, web_server_request_f func, refer_t pri)
{
	web_server_route_s *restrict route;
	if ((route = web_server_route_alloc(func, pri, 0)))
	{
		server = web_server_register_pretreat_by_route(server, route);
		refer_free(route);
		return server;
	}
	return NULL;
}

web_server_s* web_server_register_request(web_server_s *server, const char *restrict method, const char *restrict uri, web_server_request_f func, refer_t pri, web_server_request_flag_t flags)
{
	web_server_route_s *restrict route;
	if ((route = web_server_route_alloc(func, pri, flags)))
	{
		server = web_server_register_request_by_route(server, method, uri, route);
		refer_free(route);
		return server;
	}
	return NULL;
}

web_server_s* web_server_register_response(web_server_s *server, int code, web_server_request_f func, refer_t pri)
{
	web_server_route_s *restrict route;
	if ((route = web_server_route_alloc(func, pri, 0)))
	{
		server = web_server_register_response_by_route(server, code, route);
		refer_free(route);
		return server;
	}
	return NULL;
}

const web_server_route_s* web_server_find_pretreat_save(web_server_s *server)
{
	web_server_pri_s *p;
	const web_server_route_s *route;
	p = (web_server_pri_s *) server;
	yaw_lock_lock(p->register_read);
	route = (const web_server_route_s *) refer_save(p->pretreat_route);
	yaw_lock_unlock(p->register_read);
	return route;
}

const web_server_route_s* web_server_find_request_save(web_server_s *server, const char *restrict method, const char *restrict uri)
{
	web_server_pri_s *p;
	const web_server_route_s *route;
	p = (web_server_pri_s *) server;
	yaw_lock_lock(p->register_read);
	route = (const web_server_route_s *) refer_save(web_server_inner_find_request(p, method, uri));
	yaw_lock_unlock(p->register_read);
	return route;
}

const web_server_route_s* web_server_find_response_save(web_server_s *server, int code)
{
	web_server_pri_s *p;
	const web_server_route_s *route;
	p = (web_server_pri_s *) server;
	yaw_lock_lock(p->register_read);
	route = (const web_server_route_s *) refer_save(web_server_inner_find_response(p, code));
	yaw_lock_unlock(p->register_read);
	return route;
}

// thread

static inline uintptr_t web_server_inner_get_context_length(const uhttp_s *restrict http)
{
	register int64_t length;
	length = uhttp_get_header_integer_first(http, s_http_header_id_content_length);
	if (length < 0) length = 0;
	return (uintptr_t) length;
}

static inline int web_server_inner_get_is_keepalive(const uhttp_s *restrict http)
{
	uhttp_header_s *restrict c;
	c = uhttp_find_header_first(http, s_http_header_id_connection);
	return (c && !strcmp(c->value_id, "keep-alive"));
}

static inline int web_server_inner_get_is_close(const uhttp_s *restrict http)
{
	uhttp_header_s *restrict c;
	c = uhttp_find_header_first(http, s_http_header_id_connection);
	return (c && !strcmp(c->value_id, "close"));
}

static inline uhttp_s* web_server_inner_set_close(uhttp_s *restrict http)
{
	if (web_server_inner_get_is_close(http))
		return http;
	return uhttp_set_header(http, "Connection", "close");
}

static transport_s* web_server_inner_push_tp(web_server_pri_s *restrict p, transport_s *tp)
{
	__sync_fetch_and_add(&p->status.transport, 1);
	__sync_fetch_and_add(&p->status.wait_req, 1);
	if (p->q_transport->push(p->q_transport, tp))
	{
		yaw_signal_inc(p->signal);
		yaw_signal_wake(p->signal, ~0);
	}
	else
	{
		__sync_fetch_and_sub(&p->status.wait_req, 1);
		__sync_fetch_and_sub(&p->status.transport, 1);
		tp = NULL;
	}
	return tp;
}

static void web_server_working_route_do_finally(web_server_pri_s *restrict p, web_server_request_s *restrict req)
{
	if ((req->request.flags & (web_server_request_flag__req_body_discard | web_server_request_flag__attr_force_close))
		== web_server_request_flag__req_body_discard)
	{
		uintptr_t length;
		length = web_server_inner_get_context_length(req->request.request_http);
		if (req->request.request_body->used < length)
		{
			transport_attr_t ta;
			ta.running = &p->server.running;
			ta.timeout_ms = p->limit.transport_recv_timeout_ms;
			ta.flags = transport_attr_flag_do_full;
			length -= req->request.request_body->used;
			if (!exbuffer_need(req->request.request_body, length) ||
				!transport_recv(req->request.tp, req->request.request_body->data, length, NULL, &ta))
				req->request.flags |= web_server_request_flag__attr_force_close;
		}
	}
	__sync_fetch_and_sub(&p->status.transport, 1);
	if (!(req->request.flags & web_server_request_flag__attr_force_close))
		web_server_inner_push_tp(p, req->request.tp);
}

static void web_server_working_route_do_response(web_server_pri_s *restrict p, web_server_request_s *restrict req)
{
	web_transport_attr_t attr;
	transport_attr_t ta;
	if (req->request.flags & web_server_request_flag__res_route)
	{
		const web_server_route_s *restrict route;
		yaw_lock_lock(p->register_read);
		route = hashmap_get_head(&p->response_route, (uint64_t) uhttp_get_response_code(req->request.response_http));
		if (route) refer_save(route);
		yaw_lock_unlock(p->register_read);
		if (route)
		{
			route->func(&req->request, route->pri);
			refer_free(route);
		}
	}
	if (!web_server_inner_get_is_keepalive(req->request.request_http) ||
		web_server_inner_get_is_close(req->request.response_http))
		req->request.flags |= web_server_request_flag__attr_force_close;
	if ((req->request.flags & (web_server_request_flag__res_http_by_user | web_server_request_flag__attr_force_close))
		== (web_server_request_flag__attr_force_close))
		web_server_inner_set_close(req->request.response_http);
	attr.running = &p->server.running;
	attr.http_transport_timeout_ms = p->limit.transport_send_timeout_ms;
	attr.http_head_max_length = p->limit.http_max_length;
	attr.http_body_max_length = p->limit.body_max_length;
	switch (req->request.flags & (web_server_request_flag__res_http_by_user | web_server_request_flag__res_body_by_user))
	{
		case 0:
			web_transport_send_http_with_body(
				req->request.tp,
				req->request.response_http,
				req->request.request_body,
				req->request.response_body->data,
				req->request.response_body->used,
				&attr);
			break;
		case web_server_request_flag__res_body_by_user:
			web_transport_send_http(
				req->request.tp,
				req->request.response_http,
				req->request.request_body,
				&attr);
			break;
		case web_server_request_flag__res_http_by_user:
			ta.running = attr.running;
			ta.timeout_ms = attr.http_transport_timeout_ms;
			ta.flags = transport_attr_flag_do_full;
			transport_send(
				req->request.tp,
				req->request.response_body->data,
				req->request.response_body->used,
				NULL, &ta);
			break;
		default:
			break;
	}
}

static void web_server_working_route_do_request(web_server_pri_s *restrict p, transport_s *tp, uhttp_s *http)
{
	web_server_request_s *restrict req;
	const web_server_route_s *restrict route, *restrict pretreat;
	web_server_request_t *c;
	yaw_s *detach;
	refer_nstring_t method;
	refer_nstring_t uri;
	if ((method = uhttp_get_request_method(http)) &&
		(uri = uhttp_get_request_uri(http)) &&
		(req = web_server_request_alloc(&p->server, tp, http)))
	{
		if (uhttp_uri_refer_uri(req->request.request_uri, uri) &&
			(uri = uhttp_uri_get_path(req->request.request_uri)))
		{
			yaw_lock_lock(p->register_read);
			route = (const web_server_route_s *) refer_save(web_server_inner_find_request(p, method->string, uri->string));
			pretreat = (const web_server_route_s *) refer_save(p->pretreat_route);
			yaw_lock_unlock(p->register_read);
			if (pretreat)
			{
				c = pretreat->func(&req->request, pretreat->pri);
				refer_free(pretreat);
				if (!c) goto label_fail_response_500;
			}
			if (route)
			{
				req->request.flags = route->flags;
				req->route = route;
				if ((req->request.flags & web_server_request_flag__attr_mini) &&
					!uhttp_get_header_integer_first(req->request.request_http, s_http_header_id_content_length))
				{
					__sync_fetch_and_add(&p->status.working, 1);
					c = route->func(&req->request, route->pri);
					__sync_fetch_and_sub(&p->status.working, 1);
					if (c) goto label_finally;
					else goto label_fail_response_500;
				}
				else if (__sync_add_and_fetch(&p->status.detach, 1) <= p->limit.detach_number &&
					(detach = yaw_alloc_and_start(web_server_thread_detach, req, NULL)))
					refer_free(detach);
				else
				{
					__sync_fetch_and_sub(&p->status.detach, 1);
					label_fail_response_500:
					uhttp_refer_response(req->request.response_http, 500, NULL);
					label_fail_response:
					req->request.flags |= web_server_request_flag__req_body_discard | web_server_request_flag__res_route;
					label_finally:
					web_server_working_route_do_response(p, req);
					web_server_working_route_do_finally(p, req);
				}
			}
			else
			{
				uhttp_refer_response(req->request.response_http, 404, NULL);
				goto label_fail_response;
			}
		}
		refer_free(req);
	}
}

static void web_server_thread_bind(yaw_s *restrict yaw)
{
	transport_tcp_server_s *restrict listen;
	web_server_pri_s *restrict p;
	transport_s *tp;
	listen = ((web_server_bind_s *) yaw->data)->listen;
	p = ((web_server_bind_s *) yaw->data)->weak_inst;
	while (p->server.running && yaw->running)
	{
		if (transport_tcp_server_accept(listen, &tp, web_server_time_gap_msec, &p->server.running))
		{
			if (tp)
			{
				web_server_inner_push_tp(p, tp);
				refer_free(tp);
			}
		}
		else yaw_msleep(web_server_time_gap_msec);
	}
}

static void web_server_thread_working(yaw_s *restrict yaw)
{
	web_server_pri_s *restrict p;
	web_transport_poll_http_s *restrict tpoll_http;
	transport_s *tp;
	uhttp_s *http;
	uintptr_t number;
	uint32_t status;
	p = (web_server_pri_s *) yaw->data;
	tpoll_http = NULL;
	number = 0;
	while (p->server.running && yaw->running)
	{
		status = yaw_signal_get(p->signal);
		while (number < p->limit.wait_req_max_number && (p->limit.working_number * number <= p->status.wait_req) &&
			(tpoll_http || (tpoll_http = web_transport_poll_http_alloc(p->server.http_inst, p->limit.http_max_length))) &&
			(tp = (transport_s *) p->q_transport->pull(p->q_transport)))
		{
			if (web_transport_poll_http_join(tpoll_http, tp, p->limit.transport_recv_timeout_ms))
				++number;
			else
			{
				__sync_fetch_and_sub(&p->status.wait_req, 1);
				__sync_fetch_and_sub(&p->status.transport, 1);
			}
			refer_free(tp);
		}
		while (web_transport_poll_http_pop(tpoll_http, &tp, &http, NULL))
		{
			--number;
			__sync_fetch_and_sub(&p->status.wait_req, 1);
			if (http)
			{
				web_server_working_route_do_request(p, tp, http);
				refer_free(tp);
				refer_free(http);
			}
			else
			{
				__sync_fetch_and_sub(&p->status.transport, 1);
				refer_free(tp);
			}
		}
		yaw_signal_wait_time(p->signal, status, web_server_time_gap_msec * 1000);
	}
	if (tpoll_http)
		refer_free(tpoll_http);
}

static void web_server_thread_detach(yaw_s *restrict yaw)
{
	web_server_request_s *restrict req;
	web_server_pri_s *restrict p;
	const web_server_route_s *restrict route;
	req = (web_server_request_s *) yaw->pri;
	p = (web_server_pri_s *) req->request.server;
	yaw->pri = NULL;
	if (req)
	{
		if (!(req->request.flags & (web_server_request_flag__req_body_by_user | web_server_request_flag__req_body_discard)))
		{
			// recv body
			uintptr_t length;
			length = web_server_inner_get_context_length(req->request.request_http);
			if (length > 0)
			{
				transport_attr_t ta;
				ta.running = &p->server.running;
				ta.timeout_ms = req->recv_timeout_ms;
				ta.flags = transport_attr_flag_do_full;
				if (!exbuffer_need(req->request.request_body, length))
					goto label_fail;
				if (!transport_recv(req->request.tp, req->request.request_body->data, length, &req->request.request_body->used, &ta))
					goto label_fail;
			}
		}
		if (!(route = req->route) || !route->func(&req->request, route->pri))
		{
			label_fail:
			uhttp_refer_response(req->request.response_http, 500, NULL);
			req->request.flags |= web_server_request_flag__req_body_discard | web_server_request_flag__res_route;
		}
		web_server_working_route_do_response(p, req);
		web_server_working_route_do_finally(p, req);
		refer_free(req);
	}
	web_server_detach_clear(p->q_detach);
	p->q_detach->push(p->q_detach, yaw);
	__sync_fetch_and_sub(&p->status.detach, 1);
}
