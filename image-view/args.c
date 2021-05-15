#include "args.h"
#include <args.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

static args_deal_func(__, args_t *restrict)
{
	if (*index)
	{
		if (!value) return -1;
		pri->image_path = value;
		if (command) ++*index;
	}
	return 0;
}

static args_deal_func(_k, args_t *restrict)
{
	uint32_t k;
	if (!value) return -1;
	k = strtoul(value, NULL, 0);
	if (k > 15) k = 15;
	pri->multicalc = k;
	return 0;
}

static args_deal_func(_b, args_t *restrict)
{
	if (!value) return -1;
	pri->bgcolor = strtoul(value, NULL, 0);
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
		"\t*" "-h/--help             print help document\n"
		"\t*" "[--] <input:image>    input image path\n"
		"\t " "-k/--kernel <kn>      extra calc kernel [0, 15], default 3\n"
		"\t " "-b/--bgcolor <color>  background color [0x00000000, 0xffffffff], default 0xff7f7f7f\n"
		, exec
	);
}

static inline args_t* args_check(args_t *restrict args)
{
	if (args->have_help)
		goto label_null;
	if (args->image_path)
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
	args->multicalc = 3;
	args->bgcolor = 0xff7f7f7f;
	if (!hashmap_init(&a)) goto label_fail;
	if (!args_set_command(&a, "--",        (args_deal_f) __)) goto label_fail;
	if (!args_set_command(&a, "-k",        (args_deal_f) _k)) goto label_fail;
	if (!args_set_command(&a, "--kernel",  (args_deal_f) _k)) goto label_fail;
	if (!args_set_command(&a, "-b",        (args_deal_f) _b)) goto label_fail;
	if (!args_set_command(&a, "--bgcolor", (args_deal_f) _b)) goto label_fail;
	if (!args_set_command(&a, "-h",        (args_deal_f) _h)) goto label_fail;
	if (!args_set_command(&a, "--help",    (args_deal_f) _h)) goto label_fail;
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
