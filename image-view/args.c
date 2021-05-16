#include "args.h"
#include <args.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

static args_deal_func(__, args_t *restrict)
{
	if (*index)
	{
		if (pri->image_path || !value || *value == '-')
			return -1;
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
	++*index;
	return 0;
}

static args_deal_func(_b, args_t *restrict)
{
	if (!value) return -1;
	pri->bgcolor = strtoul(value, NULL, 0);
	++*index;
	return 0;
}

static args_deal_func(_s, args_t *restrict)
{
	if (!value) return -1;
	pri->shm_disable = 0;
	pri->shm_size = strtoul(value, NULL, 0);
	++*index;
	return 0;
}

static args_deal_func(_disable_shm, args_t *restrict)
{
	pri->shm_disable = 1;
	return 0;
}

static args_deal_func(_h, args_t *restrict)
{
	pri->have_help = 1;
	if (value && !strcmp(value, "full"))
	{
		pri->have_help = 2;
		++*index;
	}
	return 1;
}

static inline void args_help(const char *restrict exec, uint32_t full)
{
	printf(
		"%s [option]\n"
		"\t " "[option]              default     range                     info\n"
		"\t*" "-h/--help [full]                                            print help document\n"
		"\t*" "[--] <input:image>                                          input image path\n"
		"\t " "-k/--kernel <kn>      3           [0, 15]                   extra calc kernel\n"
		"\t " "-b/--bgcolor <color>  0xff7f7f7f  [0x00000000, 0xffffffff]  background color\n"
		"\t " "-s/--shm-size <size>  0                                     enable xcb shm and set shm size\n"
		"\t " "                                                            0 is auto by screen size\n"
		"\t " "--disable-shm         no                                    disable xcb shm\n"
		, exec
	);
	if (full) printf(
		"instructions for use:\n"
		"\t" "<Mouse_L_Button:PressMove>      move the image\n"
		"\t" "<Mouse_R_Button:PressMove>      rotate the image\n"
		"\t" "<Mouse_M_Button:Press>          reset the image\n"
		"\t" "<Mouse_M_Button:Up>             zoom in\n"
		"\t" "<Mouse_M_Button:Down>           zoom out\n"
		"\t" "<Ctrl+Mouse_L_Button:PressMove> move the window\n"
		"\t" "<Ctrl+Mouse_R_Button:PressMove> resize the window\n"
		"\t" "<Ctrl+Mouse_M_Button:Press>     reset the window by image size\n"
		"\t" "<Ctrl+Mouse_M_Button:Up>        mini zoom in\n"
		"\t" "<Ctrl+Mouse_M_Button:Down>      mini zoom out\n"
		"\t" "<Shift+Mouse_R_Button:Press>    resize the window keep width/height\n"
		"\t" "<Shift+Mouse_M_Button:Up>       mini mini zoom in\n"
		"\t" "<Shift+Mouse_M_Button:Down>     mini mini zoom out\n"
		"\t" "<Esc:Press>                     close\n"
		"\t" "<Space:Press>                   hide/show hint decorations\n"
		"\t" "<F11:Press>                     enable/disable full screen\n"
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
	if (!args_set_command(&a, "--",            (args_deal_f) __)) goto label_fail;
	if (!args_set_command(&a, "-k",            (args_deal_f) _k)) goto label_fail;
	if (!args_set_command(&a, "--kernel",      (args_deal_f) _k)) goto label_fail;
	if (!args_set_command(&a, "-b",            (args_deal_f) _b)) goto label_fail;
	if (!args_set_command(&a, "--bgcolor",     (args_deal_f) _b)) goto label_fail;
	if (!args_set_command(&a, "-s",            (args_deal_f) _s)) goto label_fail;
	if (!args_set_command(&a, "--shm-size",    (args_deal_f) _s)) goto label_fail;
	if (!args_set_command(&a, "--disable-shm", (args_deal_f) _disable_shm)) goto label_fail;
	if (!args_set_command(&a, "-h",            (args_deal_f) _h)) goto label_fail;
	if (!args_set_command(&a, "--help",        (args_deal_f) _h)) goto label_fail;
	if (!args_deal(argc, argv, &a, (args_deal_f) __, args) && args_check(args))
	{
		label_end:
		hashmap_uini(&a);
		return args;
	}
	args_help(argv[0], (args->have_help > 1));
	label_fail:
	args = NULL;
	goto label_end;
}
