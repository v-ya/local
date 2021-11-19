#include "web-server.h"
#include <transport/tcp.h>
#include <queue/queue.h>
#include <hashmap.h>
#include <yaw.h>

#define web_server_time_gap_msec  50

typedef struct web_server_bind_s web_server_bind_s;
typedef struct web_server_pri_s web_server_pri_s;

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
	hashmap_t request_route;  // method => uri => (web_server_route_t *)
	hashmap_t response_route; // code => (web_server_route_t *)
	yaw_s *working_array[];
};

static void web_server_thread_bind(yaw_s *restrict yaw);
static void web_server_thread_working(yaw_s *restrict yaw);
// static void web_server_thread_detach(yaw_s *restrict yaw);

static void web_server_route_free_func(web_server_route_s *restrict r)
{
	if (r->pri)
		refer_free(r->pri);
}

static void web_server_bind_free_func(web_server_bind_s *restrict r)
{
	if (r->thread)
		refer_free(r->thread);
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

static void web_server_pri_free_func(web_server_pri_s *restrict r)
{
	uintptr_t i;
	r->server.running = 0;
	if (r->signal)
	{
		yaw_signal_inc(r->signal);
		yaw_signal_wake(r->signal, ~0);
	}
	// release bind
	web_server_pri_clear_bind_list(r->bind_list);
	// release working
	for (i = 0; i < r->limit.working_number; ++i)
	{
		if (r->working_array[i])
			refer_free(r->working_array[i]);
	}
	// release detach (wait)
	while (r->status.detach)
		yaw_msleep(web_server_time_gap_msec);
	// release queue
	if (r->q_transport)
		refer_free(r->q_transport);
	if (r->q_detach)
		refer_free(r->q_detach);
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
	hashmap_uini(&r->request_route);
	hashmap_uini(&r->response_route);
	// release server
	if (r->server.http_inst)
		refer_free(r->server.http_inst);
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
		r->server.limit = &r->limit;
		r->server.status = &r->status;
		if (!r->server.http_inst)
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
	if ((r = (web_server_route_s *) refer_alloc(sizeof(web_server_route_s))))
	{
		r->pri = NULL;
		refer_set_free(r, (refer_free_f) web_server_route_free_func);
		r->func = func;
		r->pri = refer_save(pri);
		r->flags = flags;
	}
	return r;
}

// thread

static void web_server_thread_bind(yaw_s *restrict yaw)
{
	;
}

static void web_server_thread_working(yaw_s *restrict yaw)
{
	;
}

// static void web_server_thread_detach(yaw_s *restrict yaw)
// {
// 	;
// }
