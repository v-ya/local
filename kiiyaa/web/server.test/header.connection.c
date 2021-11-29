#include "../server/server.h"
#include <web/web-header.h>
#include <string.h>

typedef struct header_connection_s {
	inst_web_server_value_s v;
	refer_string_t connection_id;
	const char *connection_keepalive;
	const char *connection_close;
} header_connection_s;

static void header_connection_free_func(header_connection_s *restrict r)
{
	if (r->connection_id)
		refer_free(r->connection_id);
}

static uhttp_header_s* header_connection(header_connection_s *restrict r, refer_nstring_t name, web_server_request_t *restrict request)
{
	uhttp_header_s *restrict req_header;
	const char *value;
	value = r->connection_close;
	if ((req_header = uhttp_find_header_first(request->request_http, r->connection_id)) &&
		req_header->value_id && !strcmp(req_header->value_id, r->connection_keepalive))
		value = r->connection_keepalive;
	return uhttp_header_refer(name, value);
}

inst_web_server_value_s* header_alloc_connection(const web_server_s *server)
{
	header_connection_s *restrict r;
	if ((r = (header_connection_s *) refer_alloz(sizeof(header_connection_s))))
	{
		refer_set_free(r, (refer_free_f) header_connection_free_func);
		r->connection_id = refer_save(web_header_get_id(server->headers, web_header_id__connection));
		r->connection_keepalive = "keep-alive";
		r->connection_close = "close";
		r->v.value = (inst_web_server_value_f) header_connection;
	}
	return &r->v;
}
