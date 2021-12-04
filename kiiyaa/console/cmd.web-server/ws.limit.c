#include "../../web/server/server.h"
#include <console.h>
#include <mlog.h>

static console_s* limit_main(console_command_s *restrict cc, mlog_s *mlog, uintptr_t argc, const char *const argv[])
{
	web_server_s *server;
	const web_server_limit_t *restrict limit;
	if ((server = web$server$get_save()))
	{
		limit = server->limit;
		mlog_printf(mlog,
			"http_max_length:           %zu\n"
			"body_max_length:           %zu\n"
			"transport_recv_timeout_ms: %zu\n"
			"transport_send_timeout_ms: %zu\n"
			"working_number:            %zu\n"
			"detach_number:             %zu\n"
			"queue_depth_size:          %zu\n"
			"wait_req_max_number:       %zu\n"
			, limit->http_max_length
			, limit->body_max_length
			, limit->transport_recv_timeout_ms
			, limit->transport_send_timeout_ms
			, limit->working_number
			, limit->detach_number
			, limit->queue_depth_size
			, limit->wait_req_max_number);
		refer_free(server);
	}
	return NULL;
}

static void limit_limit(console_command_s *restrict cc, mlog_s *mlog, uintptr_t argc, const char *const argv[])
{
	mlog_printf(mlog, "%s\n", argv[0]);
}

console_command_s* command_alloc_limit(void)
{
	console_command_s *restrict r;
	r = (console_command_s *) refer_alloz(sizeof(console_command_s));
	if (r)
	{
		r->main = limit_main;
		r->help = limit_limit;
		r->name = "console.cmd.web-server.limit";
		r->desc = "显示 web.server 的限制信息";
	}
	return r;
}

