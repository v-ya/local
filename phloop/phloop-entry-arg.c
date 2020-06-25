#include "phloop-entry-arg.h"
#include <args.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void args_help(const char *exec, const char *type)
{
	printf(
		"%s [control] <[--] intput>\n"
		"\t*" "/--                             [input phoneme script]\n"
		"\t " "-cp/--core-path                 [core-path/*: core.dylink]\n"
		"\t " "-pp/--package-path              [package-path/*: package.json]\n"
		"\t " "-bt/--base-time                 [float > 0]\n"
		"\t " "-bv/--base-volume               [float > 0]\n"
		"\t " "-bf/--base-freline              [float > 0]\n"
		"\t " "-bfs/--base-frestep             [float != 0]\n"
		"\t " "-st/--space-time                [float]\n"
		"\t " "-sm/-sdmax/--stage-details-max  [integer > 0]\n"
		"\t " "-dm/-dmax/--details-max         [integer > 0]\n"
		"\t " "-vss/--vstack-script            [integer >= 0]\n"
		"\t " "-vsp/--vstack-phoneme           [integer >= 0]\n"
		"\t " "-sf/--sampfre                   [integer >= 0]\n"
		"\t " "-t/--thread                     [integer >= 0]\n"
		"\t " "--nice                          [float -1 ... 1]\n"
		"\t " "-xm/--xmsize                    [integer > 0]\n"
		"\t " "-tg/--time-granularity          [float 0 ... 1]\n"
		"\t " "-ut/--update-time               [float >= 0]\n"
		"\t " "-h/--help\n"
		, exec
	);
}

static args_deal_func(__, args_t*)
{
	if (*index)
	{
		if (!value || pri->input || (!command && *value == '-'))
			return -1;
		pri->input = value;
		if (command) ++*index;
	}
	return 0;
}

static args_deal_func(_cp, phloop_arg_t*)
{
	pri->core_path = value;
	++*index;
	return 0;
}

static args_deal_func(_pp, phloop_arg_t*)
{
	pri->package_path = value;
	++*index;
	return 0;
}

static args_deal_func(_bt, phloop_arg_t*)
{
	if (!value || (pri->base_time = atof(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_bv, phloop_arg_t*)
{
	if (!value || (pri->base_volume = atof(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_bf, phloop_arg_t*)
{
	if (!value || (pri->base_fre_line = atof(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_bfs, phloop_arg_t*)
{
	if (!value || !(pri->base_fre_step = atof(value))) return -1;
	++*index;
	return 0;
}

static args_deal_func(_st, phloop_arg_t*)
{
	if (!value) return -1;
	pri->space_time = atof(value);
	++*index;
	return 0;
}

static args_deal_func(_sm, phloop_arg_t*)
{
	if (!value || (int)(pri->sdmax = atoi(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_dm, phloop_arg_t*)
{
	if (!value || (int)(pri->dmax = atoi(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_vss, phloop_arg_t*)
{
	if (!value || (int)(pri->vstack_script_max = atoi(value)) < 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_vsp, phloop_arg_t*)
{
	if (!value || (int)(pri->vstack_phoneme_max = atoi(value)) < 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_sf, phloop_arg_t*)
{
	if (!value || (int)(pri->sampfre = atoi(value)) < 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_t, args_t*)
{
	if (!value || (int)(pri->thread = atoi(value)) < 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_nice, args_t*)
{
	if (!value) return -1;
	pri->nice = atof(value);
	if (pri->nice > 1) pri->nice = 1;
	else if (pri->nice < -1) pri->nice = -1;
	++*index;
	return 0;
}

static args_deal_func(_xm, phloop_arg_t*)
{
	char *s;
	size_t xm;
	if (!value) return -1;
	s = NULL;
	xm = strtoul(value, &s, 10);
	switch (*s)
	{
		case 'K':
		case 'k':
			xm <<= 10;
			break;
		case 'M':
		case 'm':
			xm <<= 20;
			break;
		case 'G':
		case 'g':
			xm <<= 30;
			break;
	}
	pri->xmsize = xm;
	++*index;
	return 0;
}

static args_deal_func(_tg, phloop_arg_t*)
{
	double v;
	if (!value) return -1;
	v = atof(value);
	if (v < 0) v = 0;
	if (v > 1) v = 1;
	pri->usleep_time = (uint32_t) (v * 1000000);
	++*index;
	return 0;
}

static args_deal_func(_ut, args_t*)
{
	double v;
	if (!value || (v = atof(value)) < 0) return -1;
	pri->update_time = (uint32_t) (v * 1000000);
	++*index;
	return 0;
}

static args_deal_func(_h, args_t*)
{
	args_help(argv[0], value);
	return 1;
}

int args_init(args_t *pri, int argc, const char *argv[])
{
	hashmap_t args;
	int r;
	memset(pri, 0, sizeof(args_t));
	pri->phloop.base_time = phoneme_script_default_base_time;
	pri->phloop.base_volume = phoneme_script_default_base_volume;
	pri->phloop.base_fre_line = phoneme_script_default_base_fre_line;
	pri->phloop.base_fre_step = phoneme_script_default_base_fre_step;
	pri->phloop.space_time = phoneme_script_default_space_time;
	pri->phloop.sdmax = phoneme_script_default_sdmax;
	pri->phloop.dmax = 8;
	pri->phloop.vstack_script_max = phoneme_script_default_vstack_script_max;
	pri->phloop.vstack_phoneme_max = phoneme_script_default_vstack_phoneme_max;
	pri->phloop.sampfre = 441000;
	pri->phloop.usleep_time = 5000;
	pri->phloop.xmsize = 1 << 20;
	pri->update_time = 500000;
	pri->nice = 1;
	r = -1;
	if (!hashmap_init(&args)) goto Err;
	if (!args_set_command(&args, "--", (args_deal_f) __)) goto Err;
	if (!args_set_command(&args, "-cp", (args_deal_f) _cp)) goto Err;
	if (!args_set_command(&args, "--core-path", (args_deal_f) _cp)) goto Err;
	if (!args_set_command(&args, "-pp", (args_deal_f) _pp)) goto Err;
	if (!args_set_command(&args, "--package-path", (args_deal_f) _pp)) goto Err;
	if (!args_set_command(&args, "-bt", (args_deal_f) _bt)) goto Err;
	if (!args_set_command(&args, "--base-time", (args_deal_f) _bt)) goto Err;
	if (!args_set_command(&args, "-bv", (args_deal_f) _bv)) goto Err;
	if (!args_set_command(&args, "--base-volume", (args_deal_f) _bv)) goto Err;
	if (!args_set_command(&args, "-bf", (args_deal_f) _bf)) goto Err;
	if (!args_set_command(&args, "--base-freline", (args_deal_f) _bf)) goto Err;
	if (!args_set_command(&args, "-bfs", (args_deal_f) _bfs)) goto Err;
	if (!args_set_command(&args, "--base-frestep", (args_deal_f) _bfs)) goto Err;
	if (!args_set_command(&args, "-st", (args_deal_f) _st)) goto Err;
	if (!args_set_command(&args, "--space-time", (args_deal_f) _st)) goto Err;
	if (!args_set_command(&args, "-sm", (args_deal_f) _sm)) goto Err;
	if (!args_set_command(&args, "-sdmax", (args_deal_f) _sm)) goto Err;
	if (!args_set_command(&args, "--stage-details-max", (args_deal_f) _sm)) goto Err;
	if (!args_set_command(&args, "-dm", (args_deal_f) _dm)) goto Err;
	if (!args_set_command(&args, "-dmax", (args_deal_f) _dm)) goto Err;
	if (!args_set_command(&args, "--details-max", (args_deal_f) _dm)) goto Err;
	if (!args_set_command(&args, "-vss", (args_deal_f) _vss)) goto Err;
	if (!args_set_command(&args, "--vstack-script", (args_deal_f) _vss)) goto Err;
	if (!args_set_command(&args, "-vsp", (args_deal_f) _vsp)) goto Err;
	if (!args_set_command(&args, "--vstack-phoneme", (args_deal_f) _vsp)) goto Err;
	if (!args_set_command(&args, "-sf", (args_deal_f) _sf)) goto Err;
	if (!args_set_command(&args, "--sampfre", (args_deal_f) _sf)) goto Err;
	if (!args_set_command(&args, "-t", (args_deal_f) _t)) goto Err;
	if (!args_set_command(&args, "--thread", (args_deal_f) _t)) goto Err;
	if (!args_set_command(&args, "--nice", (args_deal_f) _nice)) goto Err;
	if (!args_set_command(&args, "-xm", (args_deal_f) _xm)) goto Err;
	if (!args_set_command(&args, "--xmsize", (args_deal_f) _xm)) goto Err;
	if (!args_set_command(&args, "-tg", (args_deal_f) _tg)) goto Err;
	if (!args_set_command(&args, "--time-granularity", (args_deal_f) _tg)) goto Err;
	if (!args_set_command(&args, "-ut", (args_deal_f) _ut)) goto Err;
	if (!args_set_command(&args, "--update-time", (args_deal_f) _ut)) goto Err;
	if (!args_set_command(&args, "-h", (args_deal_f) _h)) goto Err;
	if (!args_set_command(&args, "--help", (args_deal_f) _h)) goto Err;
	r = args_deal(argc, argv, &args, (args_deal_f) __, pri);
	Err:
	hashmap_uini(&args);
	return r;
}

int args_check(args_t *pri)
{
	if (!pri->input || !pri->phloop.xmsize)
		return -1;
	if (pri->phloop.xmsize < 4096) pri->phloop.xmsize = 4096;
	return 0;
}
