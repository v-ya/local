#include "args.h"
#include <args.h>
#include <memory.h>
#include <stdio.h>

static args_deal_func(__, args_t *restrict)
{
	if (*index)
	{
		if (!value) return -1;
		pri->input = value;
		if (command) ++*index;
	}
	return 0;
}

static args_deal_func(_dump_samples, args_t *restrict)
{
	pri->dump_samples = 1;
	return 0;
}

static args_deal_func(_libmpeg4_verbose, args_t *restrict)
{
	pri->libmpeg4_verbose = 1;
	return 0;
}

static args_deal_func(_h, args_t *restrict)
{
	pri->have_help = 1;
	return 1;
}

static inline void args_help(const char *restrict exec)
{
	printf(
		"%s [control]\n"
		"\t*" "-h/--help           print help document\n"
		"\t*" "[--] <input:mpeg4>  input mpeg4 file\n"
		"\t " "--dump-samples      dump mpeg4's info of samples\n"
		"\t*" "--libmpeg4-verbose  print libmpeg4 verbose info\n"
		, exec
	);
}

static inline args_t* args_check(args_t *restrict args)
{
	if (args->have_help)
		goto label_null;
	if (args->input)
		goto label_ok;
	if (args->libmpeg4_verbose)
		goto label_ok;
	label_null:
	return NULL;
	label_ok:
	return args;
}

args_t* args_get(args_t *restrict args, int argc, const char *argv[])
{
	hashmap_t a;
	memset(args, 0, sizeof(*args));
	if (!hashmap_init(&a)) goto label_fail;
	if (!args_set_command(&a, "--",                 (args_deal_f) __)) goto label_fail;
	if (!args_set_command(&a, "--dump-samples",     (args_deal_f) _dump_samples)) goto label_fail;
	if (!args_set_command(&a, "--libmpeg4-verbose", (args_deal_f) _libmpeg4_verbose)) goto label_fail;
	if (!args_set_command(&a, "-h",                 (args_deal_f) _h)) goto label_fail;
	if (!args_set_command(&a, "--help",             (args_deal_f) _h)) goto label_fail;
	if (!args_deal(argc, argv, &a, (args_deal_f) __, args) && args_check(args))
	{
		label_end:
		hashmap_uini(&a);
		return args;
	}
	args_help(argv[0]);
	label_fail:
	args = NULL;
	goto label_end;
}
