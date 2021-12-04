#include "console.type.h"

typedef struct console_command_help_s {
	console_command_s cc;
	vattr_s *cmds_weak;
} console_command_help_s;

static void help_get_name_and_desc(console_command_s *restrict cc, const char **restrict name, const char **restrict desc)
{
	static const char s_null[] = "";
	if (!(*name = cc->name))
		*name = s_null;
	if (!(*desc = cc->desc))
		*desc = s_null;
}

static console_s* help_main(console_command_s *restrict cc, mlog_s *mlog, uintptr_t argc, const char *const argv[])
{
	vattr_s *restrict cmds;
	vattr_vlist_t *restrict vl;
	const char *name, *desc;
	cmds = ((console_command_help_s *) cc)->cmds_weak;
	if (argc > 1)
	{
		// help command
		if ((cc = (console_command_s *) vattr_get_first(cmds, argv[1])))
		{
			help_get_name_and_desc(cc, &name, &desc);
			mlog_printf(mlog, "%s [%s] %s\n", argv[1], name, desc);
			if (cc->help)
				cc->help(cc, mlog, argc - 1, argv + 1);
		}
		else mlog_printf(mlog, "don't find (%s)\n", argv[1]);
	}
	else
	{
		// help
		for (vl = cmds->vattr; vl; vl = vl->vattr_next)
		{
			if ((cc = (console_command_s *) vl->value))
			{
				help_get_name_and_desc(cc, &name, &desc);
				mlog_printf(mlog, "\t %-16s [%s] %s\n", vl->vslot->key, name, desc);
			}
		}
	}
	return NULL;
}

static void help_help(console_command_s *restrict cc, mlog_s *mlog, uintptr_t argc, const char *const argv[])
{
	if (argc == 1)
		mlog_printf(mlog, "%s [command ...]\n", argv[0]);
	else help_main(cc, mlog, argc, argv);
}

static console_command_s* console_command_alloc_help(vattr_s *cmds_weak)
{
	console_command_help_s *restrict r;
	r = (console_command_help_s *) refer_alloz(sizeof(console_command_help_s));
	if (r)
	{
		r->cmds_weak = cmds_weak;
		r->cc.main = help_main;
		r->cc.help = help_help;
		r->cc.name = "help";
		r->cc.desc = "get console help";
	}
	return &r->cc;
}

console_s* console_insert_command_help(console_s *restrict console)
{
	console_command_s *restrict cc;
	if ((cc = console_command_alloc_help(console->cmds)))
	{
		console = console_insert_command(console, "help", cc);
		refer_free(cc);
		return console;
	}
	return NULL;
}
