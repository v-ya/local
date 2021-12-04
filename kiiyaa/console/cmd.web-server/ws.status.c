#include "../../web/server/server.h"
#include <console.h>
#include <mlog.h>

static console_s* status_main(console_command_s *restrict cc, mlog_s *mlog, uintptr_t argc, const char *const argv[])
{
	web_server_s *server;
	const web_server_status_t *restrict status;
	if ((server = web$server$get_save()))
	{
		status = server->status;
		mlog_printf(mlog,
			"transport: %zu\n"
			"wait_req:  %zu\n"
			"working:   %zu\n"
			"detach:    %zu\n"
			, status->transport
			, status->wait_req
			, status->working
			, status->detach);
		refer_free(server);
	}
	return NULL;
}

static void status_status(console_command_s *restrict cc, mlog_s *mlog, uintptr_t argc, const char *const argv[])
{
	mlog_printf(mlog, "%s\n", argv[0]);
}

console_command_s* command_alloc_status(void)
{
	console_command_s *restrict r;
	r = (console_command_s *) refer_alloz(sizeof(console_command_s));
	if (r)
	{
		r->main = status_main;
		r->help = status_status;
		r->name = "console.cmd.web-server.status";
		r->desc = "显示 web.server 的状态信息";
	}
	return r;
}

