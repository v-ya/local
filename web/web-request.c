#include "web-request.h"
#include "web-transport.h"

static void web_request_inst_free_func(struct web_request_inst_s *restrict r)
{
	#define d_free(_m)  if (r->_m) refer_free(r->_m)
	d_free(http_inst);
	d_free(method);
	// header
	d_free(header_host);
	d_free(header_content_length);
	#undef d_free
}

struct web_request_inst_s* web_request_inst_alloc(const uhttp_inst_s *http_inst, const web_method_s *method)
{
	struct web_request_inst_s *restrict r;
	if ((r = (struct web_request_inst_s *) refer_alloz(sizeof(struct web_request_inst_s))))
	{
		refer_set_free(r, (refer_free_f) web_request_inst_free_func);
		r->http_inst = http_inst?((const uhttp_inst_s *) refer_save(http_inst)):uhttp_inst_alloc_http11_without_status();
		r->method = method?((const web_method_s *) refer_save(method)):web_method_alloc();
		r->head_limit_bytes = 128 << 10;
		r->body_limit_bytes = 8 << 20;
		r->send_timeout_ms = 3000;
		r->recv_timeout_ms = 3000;
		if (r->http_inst && r->method)
		{
			#define d_header(_m, _n)  if (!(r->header_##_m = uhttp_header_alloc(_n, NULL))) goto label_fail
			d_header(host,           "Host");
			d_header(content_length, "Content-Length");
			#undef d_header
			return r;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static void web_request_free_func(web_request_s *restrict r)
{
	#define d_free(_m)  if (r->_m) refer_free(r->_m)
	d_free(inst);
	d_free(target);
	d_free(host);
	d_free(method);
	d_free(uri);
	d_free(request);
	d_free(response);
	d_free(attch_header);
	d_free(tp);
	#undef d_free
	exbuffer_uini(&r->head_cache);
	exbuffer_uini(&r->body);
}

web_request_s* web_request_alloc(web_request_inst_s *restrict inst)
{
	web_request_s *restrict r;
	if ((r = (web_request_s *) refer_alloz(sizeof(web_request_s))))
	{
		refer_set_free(r, (refer_free_f) web_request_free_func);
		if (
			(r->inst = inst?((web_request_inst_s *) refer_save(inst)):(inst = web_request_inst_alloc(NULL, NULL))) &&
			(r->request = uhttp_alloc(inst->http_inst)) &&
			(r->response = uhttp_alloc(inst->http_inst)) &&
			exbuffer_init(&r->head_cache, 0) &&
			exbuffer_init(&r->body, 0)
		) return r;
		refer_free(r);
	}
	return NULL;
}

web_request_s* web_request_dump(web_request_s *restrict request)
{
	web_request_s *restrict r;
	if (request && (r = (web_request_s *) web_request_alloc(request->inst)))
	{
		r->target = (refer_nstring_t) refer_save(request->target);
		r->host = (refer_nstring_t) refer_save(request->host);
		r->method = (refer_nstring_t) refer_save(request->method);
		r->uri = (refer_nstring_t) refer_save(request->uri);
		r->attch_header = (uhttp_s *) refer_save(request->attch_header);
		r->tp = (transport_s *) refer_save(request->tp);
		return r;
	}
	return NULL;
}

#define d_refer(_r, _t, _m)     _r web_request_refer_##_m(web_request_s *restrict request, _t _m)\
				{\
					if (request->_m)\
						refer_free(request->_m);\
					request->_m = (_r) refer_save(_m);\
					return request->_m;\
				}
#define d_set_nstring(_m)       refer_nstring_t web_request_set_##_m(web_request_s *restrict request, const char *restrict _m)\
				{\
					refer_nstring_t v, r;\
					r = web_request_refer_##_m(request, v = refer_dump_nstring(_m));\
					if (v) refer_free(v);\
					return r;\
				}

d_refer(refer_nstring_t, refer_nstring_t, target)
d_refer(refer_nstring_t, refer_nstring_t, host)
d_refer(refer_nstring_t, refer_nstring_t, method)
d_refer(refer_nstring_t, refer_nstring_t, uri)
d_refer(uhttp_s*, uhttp_s *restrict, attch_header)
d_refer(struct transport_s*, struct transport_s *restrict, tp)

d_set_nstring(target)
d_set_nstring(host)

refer_nstring_t web_request_set_method(web_request_s *restrict request, web_method_id_t method_id)
{
	return web_request_refer_method(request, web_method_get(request->inst->method, method_id));
}

d_set_nstring(uri)

#undef d_refer
#undef d_set_nstring

struct transport_s* web_request_open_tp(web_request_s *restrict request, uintptr_t connect_timeout_ms, const volatile uintptr_t *running)
{
	transport_s *restrict tp;
	web_request_close_tp(request);
	if (request->target && (tp = transport_tcp_alloc_ipv4_connect(request->target->string, 80, NULL)))
	{
		if (transport_tcp_wait_connect(tp, (uint64_t) connect_timeout_ms, running))
			web_request_refer_tp(request, tp);
		refer_free(tp);
	}
	return request->tp;
}

void web_request_close_tp(web_request_s *restrict request)
{
	if (request->tp)
	{
		refer_free(request->tp);
		request->tp = NULL;
	}
}

web_request_s* web_request_send_request(web_request_s *restrict request, const void *body, uintptr_t body_length)
{
	uhttp_clear(request->request);
	if ((request->tp || web_request_open_tp(request, 3000, NULL)) && request->method && request->uri)
	{
		web_request_inst_s *restrict inst;
		uhttp_s *restrict http;
		inst = request->inst;
		http = request->request;
		uhttp_refer_request(http, request->method, request->uri);
		if (request->host || request->target)
		{
			if (!uhttp_set_header_refer_name(http, inst->header_host->name, request->host?request->host->string:request->target->string))
				goto label_fail;
		}
		if (request->attch_header)
		{
			if (!uhttp_append_header(http, request->attch_header))
				goto label_fail;
		}
		if (!uhttp_set_header_integer_refer_name(http, inst->header_content_length->name, (int64_t) body_length))
			goto label_fail;
		if (web_transport_send_http(request->tp, http, &request->head_cache))
		{
			if (body && body_length)
			{
				uintptr_t rn;
				if (!transport_send(request->tp, body, body_length, &rn) || rn != body_length)
					goto label_fail;
			}
			return request;
		}
	}
	label_fail:
	return NULL;
}

web_request_s* web_request_recv_response(web_request_s *restrict request, uintptr_t *restrict cost)
{
	web_request_inst_s *restrict inst;
	inst = request->inst;
	if (request->tp)
	{
		web_transport_recv_attr_t attr = {
			.running = inst->running,
			.http_head_max_length = inst->head_limit_bytes,
			.http_body_max_length = inst->body_limit_bytes,
			.http_recv_timeout_ms = inst->recv_timeout_ms
		};
		if (web_transport_recv_http_with_body(request->tp, request->response, &request->head_cache, &request->body, &attr, cost))
			return request;
	}
	return NULL;
}

web_request_s* web_request_recv_response_without_body(web_request_s *restrict request, uintptr_t *restrict cost_time_ms)
{
	web_request_inst_s *restrict inst;
	inst = request->inst;
	if (request->tp)
	{
		web_transport_recv_attr_t attr = {
			.running = inst->running,
			.http_head_max_length = inst->head_limit_bytes,
			.http_body_max_length = inst->body_limit_bytes,
			.http_recv_timeout_ms = inst->recv_timeout_ms
		};
		if (web_transport_recv_http(request->tp, request->response, &request->head_cache, &attr, cost_time_ms))
			return request;
	}
	return NULL;
}

const uint8_t* web_request_get_body_data(web_request_s *restrict request, uintptr_t *restrict length)
{
	if (length) *length = request->body.used;
	if (request->body.used)
		return (const uint8_t *) request->body.data;
	return NULL;
}

const char* web_request_get_body_string(web_request_s *restrict request)
{
	if ((exbuffer_need(&request->body, request->body.used + 1)))
	{
		request->body.data[request->body.used] = 0;
		return (const char *) request->body.data;
	}
	return NULL;
}
