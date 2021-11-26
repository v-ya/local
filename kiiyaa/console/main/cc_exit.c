#include "console_command.h"
#include <mlog.h>

typedef struct console_command_exit_s {
	console_command_s cc;
	volatile uintptr_t *running;
} console_command_exit_s;

static void exit_main(console_command_s *restrict cc, uintptr_t argc, const char *const argv[])
{
	*((console_command_exit_s *) cc)->running = 0;
}

static void exit_help(console_command_s *restrict cc, uintptr_t argc, const char *const argv[])
{
	extern mlog_s *$mlog;
	mlog_printf($mlog, "%s\n", argv[0]);
}

console_command_s* console_command_alloc_exit(volatile uintptr_t *running)
{
	if (running)
	{
		console_command_exit_s *restrict r;
		r = (console_command_exit_s *) refer_alloz(sizeof(console_command_exit_s));
		if (r)
		{
			r->running = running;
			r->cc.main = exit_main;
			r->cc.help = exit_help;
			r->cc.name = "console.main.exit";
			r->cc.desc = "退出 console.main";
			return &r->cc;
		}
	}
	return NULL;
}
