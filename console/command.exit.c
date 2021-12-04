#include "console.type.h"

typedef struct console_command_exit_s {
	console_command_s cc;
	volatile uintptr_t *running;
} console_command_exit_s;

static console_s* exit_main(console_command_s *restrict cc, mlog_s *mlog, uintptr_t argc, const char *const argv[])
{
	*((console_command_exit_s *) cc)->running = 0;
	return NULL;
}

static void exit_help(console_command_s *restrict cc, mlog_s *mlog, uintptr_t argc, const char *const argv[])
{
	mlog_printf(mlog, "%s\n", argv[0]);
}

static console_command_s* console_command_alloc_exit(volatile uintptr_t *running)
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
			r->cc.name = "exit";
			r->cc.desc = "exit console";
			return &r->cc;
		}
	}
	return NULL;
}

console_s* console_insert_command_exit(console_s *restrict console)
{
	console_command_s *restrict cc;
	if ((cc = console_command_alloc_exit(&console->running)))
	{
		console = console_insert_command(console, "exit", cc);
		refer_free(cc);
		return console;
	}
	return NULL;
}
