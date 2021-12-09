#include "server.h"

#define d_flags_modify(_f)  \
	static web_server_request_flag_t inst_web_server_flags__##_f(web_server_request_flag_t flags)\
	{\
		return flags | web_server_request_flag__##_f;\
	}

d_flags_modify(req_body_by_user)
d_flags_modify(req_body_discard)
d_flags_modify(res_http_by_user)
d_flags_modify(res_body_by_user)
d_flags_modify(res_route)
d_flags_modify(attr_mini)
d_flags_modify(attr_force_close)

#undef d_flags_modify

static hashmap_t* inst_web_server_initial_flags(hashmap_t *restrict hm)
{
	#define d_set_flags(_n, _f)  hashmap_set_name(hm, _n, inst_web_server_flags__##_f, NULL)
	if (
		d_set_flags("req.body.by-user", req_body_by_user) &&
		d_set_flags("req.body.discard", req_body_discard) &&
		d_set_flags("res.http.by-user", res_http_by_user) &&
		d_set_flags("res.body.by-user", res_body_by_user) &&
		d_set_flags("res.route", res_route) &&
		d_set_flags("res.attr.mini", attr_mini) &&
		d_set_flags("res.attr.force-close", attr_force_close)
	) return hm;
	return NULL;
	#undef d_set_flags
}

static void inst_web_server_free_func(inst_web_server_s *restrict r)
{
	if (r->server)
		refer_free(r->server);
	if (r->method)
		refer_free(r->method);
	if (r->pretreat)
		refer_free(r->pretreat);
	hashmap_uini(&r->tag);
	hashmap_uini(&r->flags);
}

web_server_request_t* web_server_pretreat(web_server_request_t *restrict request, refer_t pri);
refer_t pretreat_alloc(const struct web_header_s *restrict headers);

inst_web_server_s* inst_web_server_alloc(const web_server_limit_t *restrict limit)
{
	inst_web_server_s *restrict r;
	if ((r = (inst_web_server_s *) refer_alloz(sizeof(inst_web_server_s))))
	{
		refer_set_free(r, (refer_free_f) inst_web_server_free_func);
		if (hashmap_init(&r->tag) && hashmap_init(&r->flags) &&
			inst_web_server_initial_flags(&r->flags) &&
			(r->method = web_method_alloc()) &&
			(r->server = web_server_alloc(NULL, limit)) &&
			(r->pretreat = pretreat_alloc(r->server->headers)) &&
			web_server_register_pretreat(r->server, web_server_pretreat, r->pretreat))
			return r;
		refer_free(r);
	}
	return NULL;
}
