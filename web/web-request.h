#ifndef _web_request_h_
#define _web_request_h_

#include <refer.h>
#include <uhttp.h>
#include <exbuffer.h>
#include "web-method.h"

struct transport_s;
struct web_dns_s;

typedef const struct web_request_inst_s {
	// inst refer
	const uhttp_inst_s *http_inst;
	const web_method_s *method;
	// user set
	const volatile uintptr_t *running;
	uintptr_t head_limit_bytes;
	uintptr_t body_limit_bytes;
	uintptr_t connect_timeout_ms;
	uintptr_t send_timeout_ms;
	uintptr_t recv_timeout_ms;
	// header
	uhttp_header_s *header_host;
	uhttp_header_s *header_content_length;
} web_request_inst_s;

typedef struct web_request_s {
	web_request_inst_s *inst;
	refer_nstring_t target;
	refer_nstring_t host;
	refer_nstring_t method;
	refer_nstring_t uri;
	uhttp_s *request;
	uhttp_s *response;
	uhttp_s *attch_header;
	struct transport_s *tp;
	exbuffer_t head_cache;
	exbuffer_t body;
} web_request_s;

struct web_request_inst_s* web_request_inst_alloc(const uhttp_inst_s *http_inst, const web_method_s *method);

web_request_s* web_request_alloc(web_request_inst_s *restrict inst);
web_request_s* web_request_dump(web_request_s *restrict request);
void web_request_clear(web_request_s *restrict request);

refer_nstring_t web_request_refer_target(web_request_s *restrict request, refer_nstring_t target);
refer_nstring_t web_request_refer_host(web_request_s *restrict request, refer_nstring_t host);
refer_nstring_t web_request_refer_method(web_request_s *restrict request, refer_nstring_t method);
refer_nstring_t web_request_refer_uri(web_request_s *restrict request, refer_nstring_t uri);
uhttp_s* web_request_refer_attch_header(web_request_s *restrict request, uhttp_s *restrict attch_header);
struct transport_s* web_request_refer_tp(web_request_s *restrict request, struct transport_s *restrict tp);

refer_nstring_t web_request_set_target(web_request_s *restrict request, const char *restrict target);
refer_nstring_t web_request_set_host(web_request_s *restrict request, const char *restrict host);
refer_nstring_t web_request_set_method(web_request_s *restrict request, web_method_id_t method_id);
refer_nstring_t web_request_set_uri(web_request_s *restrict request, const char *restrict uri);
refer_nstring_t web_request_set_ip_and_port(web_request_s *restrict request, const char *restrict ip, uint32_t port);
web_request_s* web_request_set_full_uri(web_request_s *restrict request, struct web_dns_s *restrict dns, const char *restrict full_uri);

struct transport_s* web_request_open_tp(web_request_s *restrict request);
void web_request_close_tp(web_request_s *restrict request);

web_request_s* web_request_send_request(web_request_s *restrict request, const void *body, uintptr_t body_length);
web_request_s* web_request_recv_response(web_request_s *restrict request);
web_request_s* web_request_recv_response_without_body(web_request_s *restrict request);

web_request_s* web_request_recv_request(web_request_s *restrict request);
web_request_s* web_request_recv_request_without_body(web_request_s *restrict request);
web_request_s* web_request_send_response(web_request_s *restrict request, const void *body, uintptr_t body_length);

const uint8_t* web_request_get_body_data(web_request_s *restrict request, uintptr_t *restrict length);
const char* web_request_get_body_string(web_request_s *restrict request);

#endif
