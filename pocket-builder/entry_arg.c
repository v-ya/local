#include "entry_arg.h"
#include <args.h>
#include <stdio.h>

void args_help(const char *exec)
{
	printf(
		"%s <[--] intput> [-o output]\n"
		"\t*" "--         [input builder script | pocket]\n"
		"\t " "-o         [output pocket]\n"
		"\t " "--kiya     [kiya pocket path]\n"
		"\t*" "-h/--help\n"
		, exec
	);
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
	if (!value || pri->output) return -1;
	pri->output = value;
	++*index;
	return 0;
}

static args_deal_func(__kiya, args_t*)
{
	if (!value || pri->kiya_pocket_path) return -1;
	pri->kiya_pocket_path = value;
	++*index;
	return 0;
}

static args_deal_func(_h, args_t*)
{
	return 1;
}

int args_init(args_t *pri, int argc, const char *argv[])
{
	hashmap_t args;
	int r;
	pri->input = NULL;
	pri->output = NULL;
	pri->kiya_pocket_path = NULL;
	pri->kiya_xmsize = (1 << 20);
	r = -1;
	if (!hashmap_init(&args)) goto Err;
	if (!args_set_command(&args, "--",     (args_deal_f) __)) goto Err;
	if (!args_set_command(&args, "-o",     (args_deal_f) _o)) goto Err;
	if (!args_set_command(&args, "--kiya", (args_deal_f) __kiya)) goto Err;
	if (!args_set_command(&args, "-h",     (args_deal_f) _h)) goto Err;
	if (!args_set_command(&args, "--help", (args_deal_f) _h)) goto Err;
	r = args_deal(argc, argv, &args, (args_deal_f) __, pri);
	Err:
	hashmap_uini(&args);
	return r;
}

