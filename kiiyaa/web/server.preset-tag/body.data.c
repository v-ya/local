#include "../server/server.h"
#include <web/web-transport.h>
#include <web/web-header.h>

typedef struct tag_s {
	inst_web_server_tag_s tag;
	web_header_s *headers;
} tag_s;

typedef struct body_data_s {
	pocket_s *pocket;
	const pocket_attr_t *attr;
	uhttp_header_s *header_context_length;
} body_data_s;

static void tag_free_func(tag_s *restrict r)
{
	if (r->headers)
		refer_free(r->headers);
}

static void body_data_free_func(body_data_s *restrict r)
{
	if (r->pocket)
		refer_free(r->pocket);
	if (r->header_context_length)
		refer_free(r->header_context_length);
}

static refer_t body_data_allocer(tag_s *restrict tag, pocket_s *restrict pocket, const pocket_attr_t *restrict item)
{
	body_data_s *restrict r;
	if ((r = (body_data_s *) refer_alloz(sizeof(body_data_s))))
	{
		refer_set_free(r, (refer_free_f) body_data_free_func);
		r->header_context_length = uhttp_header_refer_integer(
			web_header_get_name(tag->headers, web_header_id__content_length),
			(int64_t) item->size);
		if (r->header_context_length)
		{
			r->pocket = (pocket_s *) refer_save(pocket);
			r->attr = item;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static web_server_request_t* body_data_request(web_server_request_t *restrict request)
{
	const body_data_s *restrict p;
	const web_server_s *restrict server;
	web_transport_attr_t ta;
	p = (const body_data_s *) request->pri;
	server = request->server;
	ta.running = &server->running;
	ta.http_transport_timeout_ms = server->limit->transport_send_timeout_ms;
	ta.http_head_max_length = server->limit->http_max_length;
	ta.http_body_max_length = ~(uintptr_t) 0;
	if (uhttp_set_response(request->response_http, 200, NULL) &&
		uhttp_refer_header_tail(request->response_http, p->header_context_length))
	{
		request->flags |= web_server_request_flag__res_http_by_user | web_server_request_flag__res_body_by_user;
		if (!web_transport_send_http_with_body(
			request->tp,
			request->response_http,
			request->request_body,
			p->attr->data.ptr,
			(uintptr_t) p->attr->size,
			&ta)) request->flags |= web_server_request_flag__attr_force_close;
		return request;
	}
	return NULL;
}

static web_server_request_flag_t body_data_parse_flags(const char *restrict name)
{
	web_server_request_flag_t flags;
	flags = web_server_request_flag__req_body_discard;
	do {
		while (*name && *name != '|') ++name;
		if (*name && *++name)
		{
			if (
				name[0] == 'm' &&
				name[1] == 'i' &&
				name[3] == 'n' &&
				name[4] == 'i' &&
				(!name[5] || name[5] == '|'))
			{
				flags |= web_server_request_flag__attr_mini;
				name += 5;
			}
			else if (
				name[0] == 'c' &&
				name[1] == 'l' &&
				name[3] == 'o' &&
				name[4] == 's' &&
				name[5] == 'e' &&
				(!name[6] || name[6] == '|'))
			{
				flags |= web_server_request_flag__attr_force_close;
				name += 6;
			}
		}
	} while (*name);
	return flags;
}

inst_web_server_tag_s* body_data_builder(const web_server_s *server, pocket_s *restrict pocket, const pocket_attr_t *restrict item)
{
	tag_s *restrict r;
	r = (tag_s *) refer_alloz(sizeof(tag_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) tag_free_func);
		r->tag.request_func = body_data_request;
		r->tag.allocer = (inst_web_server_tag_allocer_f) body_data_allocer;
		r->tag.request_flag = body_data_parse_flags(item->name.string);
		r->headers = (web_header_s *) refer_save(server->headers);
	}
	return &r->tag;
}
