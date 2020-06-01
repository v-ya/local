#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <args.h>
#include <wav.h>

typedef struct args_t {
	const char *input;
	const char *output;
	uint32_t channels;
	uint32_t sampfre;
} args_t;

static args_deal_func(__, args_t*)
{
	if (!value) return -1;
	pri->input = value;
	if (command) ++*index;
	return 0;
}

static args_deal_func(_o, args_t*)
{
	if (!value) return -1;
	pri->output = value;
	++*index;
	return 0;
}

static args_deal_func(_c, args_t*)
{
	if (!value) return -1;
	pri->channels = atoi(value);
	++*index;
	return 0;
}

static args_deal_func(_s, args_t*)
{
	if (!value) return -1;
	pri->sampfre = atoi(value);
	++*index;
	return 0;
}

static args_deal_func(_h, args_t*)
{
	return 1;
}

int args(args_t *pri, int argc, const char *argv[])
{
	hashmap_t a;
	int r;
	r = -1;
	if (!hashmap_init(&a)) goto Err;
	if (!args_set_command(&a, "--", (args_deal_f) __)) goto Err;
	if (!args_set_command(&a, "-o", (args_deal_f) _o)) goto Err;
	if (!args_set_command(&a, "-c", (args_deal_f) _c)) goto Err;
	if (!args_set_command(&a, "-s", (args_deal_f) _s)) goto Err;
	if (!args_set_command(&a, "-h", (args_deal_f) _h)) goto Err;
	if (!args_set_command(&a, "--help", (args_deal_f) _h)) goto Err;
	r = args_deal(argc, argv, &a, (args_deal_f) __, pri);
	Err:
	hashmap_uini(&a);
	return r;
}

int main(int argc, const char *argv[])
{
	wav_load_t *wl;
	args_t r;
	int ret;
	r.input = r.output = NULL;
	r.channels = 1;
	r.sampfre = 96000;
	if (argc > 1)
	{
		ret = args(&r, argc, argv);
		if (ret > 0) goto label_help;
		else if (ret < 0) goto Err;
		if (!r.input || !r.output) goto Err;
		ret = -1;
		wl = pcms32le_load(r.input, r.channels, r.sampfre);
		if (wl)
		{
			ret = wav_save_load(r.output, wl);
			if (ret) printf("save wav[%s] fail\n", r.output);
			wav_free(wl);
		}
		else printf("don't load pcms32le file [%s]\n", r.input);
	}
	else
	{
		label_help:
		printf("%s <input:pcms32le> <-o output:wav> [-c channels (1)] [-s sampfre (96000)]\n", argv[0]);
		ret = 0;
	}
	return ret;
	Err:
	printf("please use '%s -h/--help'\n", argv[0]);
	return -1;
}
