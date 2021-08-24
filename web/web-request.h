#ifndef _web_request_h_
#define _web_request_h_

#include <refer.h>
#include <uhttp.h>
#include <exbuffer.h>
#include "web-method.h"

struct transport_s;

typedef const struct web_request_inst_s {
	const uhttp_inst_s *http_inst;
	const web_method_s *method;
} web_request_inst_s;

typedef struct web_request_s {
	web_request_inst_s *inst;
	refer_nstring_t ip;
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

#endif
