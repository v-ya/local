#ifndef _kiiyaa_web_server__server_h_
#define _kiiyaa_web_server__server_h_

#include <refer.h>
#include <hashmap.h>
#include <pocket/pocket.h>
#include <web/web-server.h>
#include <web/web-method.h>

typedef struct inst_web_server_s inst_web_server_s;
typedef struct inst_web_server_tag_s inst_web_server_tag_s;
typedef struct inst_web_server_value_s inst_web_server_value_s;

typedef inst_web_server_tag_s* (*inst_web_server_tag_builder_f)(const web_server_s *server, pocket_s *restrict pocket, const pocket_attr_t *restrict item);
typedef refer_t (*inst_web_server_tag_allocer_f)(inst_web_server_tag_s *restrict r, pocket_s *restrict pocket, const pocket_attr_t *restrict item);
typedef web_server_request_flag_t (*inst_web_server_flags_modify_f)(web_server_request_flag_t flags);

typedef inst_web_server_value_s* (*inst_web_server_value_allocer_f)(const web_server_s *server);
typedef uhttp_header_s* (*inst_web_server_value_f)(inst_web_server_value_s *restrict r, refer_nstring_t name, web_server_request_t *restrict request);

struct inst_web_server_value_s {
	// return save(uhttp_header_s *)
	inst_web_server_value_f value;
};

struct inst_web_server_tag_s {
	const web_server_route_s *route;
	web_server_request_f request_func;
	inst_web_server_tag_allocer_f allocer;
	web_server_request_flag_t request_flag;
};

struct inst_web_server_s {
	web_server_s *server;  // only web server inst
	web_method_s *method;  // parse used
	hashmap_t tag;         // => (inst_web_server_tag_s *)
	hashmap_t flags;       // => (inst_web_server_flags_modify_f)
	refer_t pretreat;
};

web_server_s* web$server$get_save(void);

#endif
