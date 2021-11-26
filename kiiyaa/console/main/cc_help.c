#include "console_command.h"
#include <mlog.h>
#include <vattr.h>

extern mlog_s *$mlog;
extern vattr_s *ccpool;

static void help_get_name_and_desc(console_command_s *restrict cc, const char **restrict name, const char **restrict desc)
{
	static const char s_null[] = "";
	if (!(*name = cc->name))
		*name = s_null;
	if (!(*desc = cc->desc))
		*desc = s_null;
}

static void help_main(console_command_s *restrict cc, uintptr_t argc, const char *const argv[])
{
	vattr_vlist_t *restrict vl;
	const char *name, *desc;
	if (argc > 1)
	{
		// help command
		if ((cc = (console_command_s *) vattr_get_first(ccpool, argv[1])))
		{
			help_get_name_and_desc(cc, &name, &desc);
			mlog_printf($mlog, "%s [%s] %s\n", argv[1], name, desc);
			if (cc->help)
				cc->help(cc, argc - 1, argv + 1);
		}
		else mlog_printf($mlog, "don't find (%s)\n", argv[1]);
	}
	else
	{
		// help
		for (vl = ccpool->vattr; vl; vl = vl->vattr_next)
		{
			if ((cc = (console_command_s *) vl->value))
			{
				help_get_name_and_desc(cc, &name, &desc);
				mlog_printf($mlog, "\t %-16s [%s] %s\n", vl->vslot->key, name, desc);
			}
		}
	}
}

static void help_help(console_command_s *restrict cc, uintptr_t argc, const char *const argv[])
{
	mlog_printf($mlog, "%s [command]\n", argv[0]);
}

console_command_s* console_command_alloc_help(void)
{
	console_command_s *restrict r;
	r = (console_command_s *) refer_alloz(sizeof(console_command_s));
	if (r)
	{
		r->main = help_main;
		r->help = help_help;
		r->name = "console.main.help";
		r->desc = "获取 console.main 的帮助";
	}
	return r;
}
