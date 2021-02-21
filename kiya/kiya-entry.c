#include "kiya.h"
#include <pocket/pocket-verify.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct inst_t {
	mlog_s *restrict mlog;
	kiya_t *restrict kiya;
	pocket_verify_s *restrict verify;
	uintptr_t xmsize;
	const char *name;
	const char *main;
} inst_t;

typedef inst_t* (*args_deal_f)(inst_t *restrict inst, const char *restrict name, const char *restrict value);

static inst_t* arg_dash(inst_t *restrict inst, uintptr_t argc, const char *argv[], args_deal_f dash)
{
	register const char *restrict value;
	const char *restrict name;
	uintptr_t i;
	name = NULL;
	for (i = 0; i < argc; ++i)
	{
		value = argv[i];
		if (*value != ':')
		{
			if (!dash(inst, name, value))
				return NULL;
		}
		else if (value[1] == ':')
			name = NULL;
		else name = value + 1;
	}
	return inst;
}

static inst_t* arg_set(inst_t *restrict inst, const char *restrict name, const char *restrict value)
{
	if (!name || kiya_set_arg(inst->kiya, name, value))
		return inst;
	return NULL;
}

static inst_t* arg_load(inst_t *restrict inst, const char *restrict name, const char *restrict value)
{
	if (!name)
	{
		pocket_s *restrict pocket;
		if (!(pocket = pocket_load(value, inst->verify)))
			goto label_fail;
		if (!kiya_load(inst->kiya, pocket))
			inst = NULL;
		refer_free(pocket);
	}
	return inst;
	label_fail:
	mlog_printf(inst->mlog, "kiya pocket(%s) load fail\n", value);
	return NULL;
}

static const char ** main_arg_go(inst_t *restrict inst, uintptr_t argc, const char **argv, uintptr_t *restrict rargc)
{
	if (argc <= 1)
	{
		printf(
			"%s [options] [--] ...\n"
			"\t" "call [name].main\n"
			"options:\n"
			"\t" "--xmsize  <xmsize>\n"
			"\t" "--name    <name>\n"
			"\t" "--main    <main>\n"
			"args:\n"
			"\t" ":name     start [name] arg setting\n"
			"\t" "::        stop arg setting\n"
			"other:\n"
			"\t" "<kiya pocket>\n"
			, argv[0]);
		return NULL;
	}
	while (--argc)
	{
		register const char *restrict s;
		s = *++argv;
		if (s[0] != '-' || s[1] != '-') break;
		s += 2;
		if (!*s)
		{
			--argc;
			++argv;
			break;
		}
		if (!strcmp(s, "xmsize"))
		{
			--argc;
			inst->xmsize = strtoul(*++argv, NULL, 0);
		}
		else if (!strcmp(s, "name"))
		{
			--argc;
			inst->name = *++argv;
		}
		else if (!strcmp(s, "main"))
		{
			--argc;
			inst->main = *++argv;
		}
		else break;
	}
	*rargc = argc;
	return argv;
}

int main(int argc, const char *argv[])
{
	inst_t inst;
	uintptr_t rargc;
	const char **rargv;
	int ret = 0;
	inst.xmsize = (1 << 20);
	inst.name = NULL;
	inst.main = "main";
	if ((rargv = main_arg_go(&inst, (uintptr_t) argc, argv, &rargc)))
	{
		ret = -1;
		inst.mlog = mlog_alloc(0);
		inst.kiya = kiya_alloc(inst.mlog, inst.xmsize);
		inst.verify = pocket_verify_default();
		if (inst.mlog && inst.kiya && inst.verify)
		{
			mlog_set_report(inst.mlog, mlog_report_stdout_func, NULL);
			if (arg_dash(&inst, rargc, rargv, arg_set) &&
				arg_dash(&inst, rargc, rargv, arg_load))
			{
				if (!kiya_do(inst.kiya, inst.name, inst.main, &ret))
				{
					mlog_printf(inst.mlog, "call %c%s%c%c%s fail\n",
						inst.name?'[':0,
						inst.name?inst.name:"",
						inst.name?']':0,
						inst.name?'.':0,
						inst.main
					);
				}
			}
		}
		if (inst.mlog) refer_free(inst.mlog);
		if (inst.kiya) kiya_free(inst.kiya);
		if (inst.verify) refer_free(inst.verify);
	}
	return ret;
}
