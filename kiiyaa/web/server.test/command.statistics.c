#include "statistics.h"
#include "../../console/main/console_command.h"
#include <mlog.h>

typedef struct console_command_statistics_s {
	console_command_s cc;
	statistics_s *statistics;
} console_command_statistics_s;

static void console_command_statistics_free_func(console_command_statistics_s *restrict r)
{
	if (r->statistics)
		refer_free(r->statistics);
}

static void statistics_main(console_command_s *restrict cc, uintptr_t argc, const char *const argv[])
{
	statistics_dump(((console_command_statistics_s *) cc)->statistics);
}

static void statistics_help(console_command_s *restrict cc, uintptr_t argc, const char *const argv[])
{
	extern mlog_s *$mlog;
	mlog_printf($mlog, "%s\n", argv[0]);
}

console_command_s* command_alloc_statistics(void)
{
	console_command_statistics_s *restrict r;
	r = NULL;
	if (!statistics) statistics = statistics_alloc();
	if (statistics && (r = (console_command_statistics_s *) refer_alloz(sizeof(console_command_statistics_s))))
	{
		refer_set_free(r, (refer_free_f) console_command_statistics_free_func);
		r->statistics = (statistics_s *) refer_save(statistics);
		r->cc.main = statistics_main;
		r->cc.help = statistics_help;
		r->cc.name = "web.server.test.statistics";
		r->cc.desc = "显示 web.server.test 的访问测试数据";
	}
	return &r->cc;
}
