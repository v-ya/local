#ifndef _web_server_h_
#define _web_server_h_

#include <refer.h>
#include <uhttp.h>
#include <exbuffer.h>
#include <transport/transport.h>

struct web_header_s;

typedef struct web_server_s web_server_s;
typedef struct web_server_route_s web_server_route_s;
typedef struct web_server_limit_t web_server_limit_t;
typedef struct web_server_status_t web_server_status_t;
typedef struct web_server_request_t web_server_request_t;

typedef enum web_server_request_flag_t web_server_request_flag_t;

typedef web_server_request_t* (*web_server_request_f)(web_server_request_t *restrict request, refer_t pri);

enum web_server_request_flag_t {
	// (web_server_s) 负责接收 body => (web_server_request_t).request_body
	web_server_request_flag__req_body_by_inst = 0x0000,
	// (register) 负责接收 body
	web_server_request_flag__req_body_by_user = 0x0001,
	// (register) 不处理 body，处理完成后由 (web_server_s) 负责舍弃 body
	web_server_request_flag__req_body_discard = 0x0002,

	// (web_server_s) 负责发送 http <= (web_server_request_t).response_http
	web_server_request_flag__res_http_by_inst = 0x0000,
	// (register) 负责发送 http
	web_server_request_flag__res_http_by_user = 0x0010,
	// (web_server_s) 负责发送 body <= (web_server_request_t).response_body
	web_server_request_flag__res_body_by_inst = 0x0000,
	// (register) 负责发送 body
	web_server_request_flag__res_body_by_user = 0x0020,
	// (register) 只负责更改 code 返回状态，由 (web_server_s) 依据 code 重新路由
	web_server_request_flag__res_route        = 0x0040,

	// (web_server_s) 为该请求使用工作线程；默认分配独立线程
	web_server_request_flag__attr_mini        = 0x0100,
	// (web_server_s) 将强制关闭传输当该次请求结束，而忽略 "Connection" 字段；默认根据 "Connection" 字段决定
	web_server_request_flag__attr_force_close = 0x0200,
};

struct web_server_limit_t {
	// http 请求的 header 最大限制
	uintptr_t http_max_length;
	// http 请求的 body 最大限制
	uintptr_t body_max_length;
	// (web_server_s) 所负责的 接收行为 的超时时间
	uintptr_t transport_recv_timeout_ms;
	// (web_server_s) 所负责的 发送行为 的超时时间
	uintptr_t transport_send_timeout_ms;
	// 工作线程的数目
	uintptr_t working_number;
	// 独立线程同时所允许的最大数目
	uintptr_t detach_number;
	// 请求分配队列的大小
	uintptr_t queue_depth_size;
	// 单个工作线程中同时等待请求的最大数量限制
	uintptr_t wait_req_max_number;
};

struct web_server_status_t {
	// 当前的连接数
	volatile uintptr_t transport;
	// 当前等待解析请求头的数目
	volatile uintptr_t wait_req;
	// 当前工作线程中正在处理的数目
	volatile uintptr_t working;
	// 当前独立线程的数目
	volatile uintptr_t detach;
};

struct web_server_request_t {
	const web_server_s *server;
	transport_s *tp;
	uhttp_uri_s *request_uri;
	uhttp_s *request_http;
	uhttp_s *response_http;
	exbuffer_t *request_body;
	exbuffer_t *response_body;
	web_server_request_flag_t flags;
};

struct web_server_s {
	volatile uintptr_t running;
	const uhttp_inst_s *http_inst;
	const struct web_header_s *headers;
	const web_server_limit_t *limit;
	const web_server_status_t *status;
};

web_server_s* web_server_alloc(const uhttp_inst_s *restrict http_inst, const web_server_limit_t *restrict limit);
web_server_s* web_server_add_bind(web_server_s *server, const char *restrict ip, uint32_t port, uint32_t nlisten);
web_server_route_s* web_server_route_alloc(web_server_request_f func, refer_t pri, web_server_request_flag_t flags);
web_server_s* web_server_register_pretreat_by_route(web_server_s *server, const web_server_route_s *restrict route);
web_server_s* web_server_register_request_by_route(web_server_s *server, const char *restrict method, const char *restrict uri, const web_server_route_s *restrict route);
web_server_s* web_server_register_response_by_route(web_server_s *server, int code, const web_server_route_s *restrict route);
web_server_s* web_server_register_pretreat(web_server_s *server, web_server_request_f func, refer_t pri);
web_server_s* web_server_register_request(web_server_s *server, const char *restrict method, const char *restrict uri, web_server_request_f func, refer_t pri, web_server_request_flag_t flags);
web_server_s* web_server_register_response(web_server_s *server, int code, web_server_request_f func, refer_t pri);
const web_server_route_s* web_server_find_pretreat_save(web_server_s *server);
const web_server_route_s* web_server_find_request_save(web_server_s *server, const char *restrict method, const char *restrict uri);
const web_server_route_s* web_server_find_response_save(web_server_s *server, int code);

#endif
