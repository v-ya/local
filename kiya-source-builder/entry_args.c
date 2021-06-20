#include "entry_args.h"
#include <args.h>
#include <stdio.h>
#include <memory.h>

static const char* string_check_null(const char *restrict s)
{
	if (strcmp(s, "null"))
		return s;
	return NULL;
}

static args_deal_func(__, args_t*)
{
	if (*index)
	{
		if (pri->input || !value || (!command && *value == '-'))
			return -1;
		pri->input = value;
		if (command) ++*index;
	}
	return 0;
}

static args_deal_func(_o, args_t*)
{
	if (!value) return -1;
	pri->output = value;
	++*index;
	return 0;
}

static args_deal_func(__verify, args_t*)
{
	if (!value) return -1;
	pri->verify = string_check_null(value);
	++*index;
	return 0;
}

static args_deal_func(__version, args_t*)
{
	if (!value) return -1;
	pri->version = string_check_null(value);
	++*index;
	return 0;
}

static args_deal_func(__author, args_t*)
{
	if (!value) return -1;
	pri->author = string_check_null(value);
	++*index;
	return 0;
}

static args_deal_func(__time, args_t*)
{
	if (!value) return -1;
	pri->time = string_check_null(value);
	++*index;
	return 0;
}

static args_deal_func(__description, args_t*)
{
	if (!value) return -1;
	pri->description = string_check_null(value);
	++*index;
	return 0;
}

static args_deal_func(__flag, args_t*)
{
	if (!value) return -1;
	pri->flag = string_check_null(value);
	++*index;
	return 0;
}

static args_deal_func(__script, args_t*)
{
	pri->o_builder_srcipt = 1;
	return 0;
}

static args_deal_func(_h, args_t*)
{
	return 1;
}

static void args_help(const char *exec)
{
	printf(
		"%s <[--] intput>     [-o output]\n"
		"\t*" "--             [input source full path]\n"
		"\t " "-o             [output source pocket | pocket builder script]\n"
		"\t " "--verify       [attr::verify, default: \"*>^~64.4\"]\n"
		"\t " "--version      [attr::version]\n"
		"\t " "--author       [attr::author]\n"
		"\t " "--time         [attr::time, default: now]\n"
		"\t " "--description  [attr::description text file path]\n"
		"\t " "--flag         [attr::flag]\n"
		"\t " "--script       [force output format is builder script]\n"
		"\t*" "-h/--help\n"
		, exec
	);
}

static void args_init(args_t *restrict pri)
{
	memset(pri, 0, sizeof(*pri));
	pri->verify = "*>^~64.4";
}

static args_t* args_check(args_t *restrict pri)
{
	if (pri->input)
	{
		if (!pri->output)
			pri->o_builder_srcipt = 1;
		return pri;
	}
	return NULL;
}

args_t* args_get(args_t *restrict pri, int argc, const char *argv[])
{
	hashmap_t args;
	int r;
	r = -1;
	args_init(pri);
	if (!hashmap_init(&args)) goto label_fail;
	if (!args_set_command(&args, "--",            (args_deal_f) __))            goto label_fail;
	if (!args_set_command(&args, "-o",            (args_deal_f) _o))            goto label_fail;
	if (!args_set_command(&args, "--verify",      (args_deal_f) __verify))      goto label_fail;
	if (!args_set_command(&args, "--version",     (args_deal_f) __version))     goto label_fail;
	if (!args_set_command(&args, "--author",      (args_deal_f) __author))      goto label_fail;
	if (!args_set_command(&args, "--time",        (args_deal_f) __time))        goto label_fail;
	if (!args_set_command(&args, "--description", (args_deal_f) __description)) goto label_fail;
	if (!args_set_command(&args, "--flag",        (args_deal_f) __flag))        goto label_fail;
	if (!args_set_command(&args, "--script",      (args_deal_f) __script))      goto label_fail;
	if (!args_set_command(&args, "-h",            (args_deal_f) _h))            goto label_fail;
	if (!args_set_command(&args, "--help",        (args_deal_f) _h))            goto label_fail;
	r = args_deal(argc, argv, &args, (args_deal_f) __, pri);
	label_fail:
	hashmap_uini(&args);
	if (!r && args_check(pri))
		return pri;
	args_help(argv[0]);
	return NULL;
}

