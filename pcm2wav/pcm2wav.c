#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wav.h>

static uint64_t argv_s2u(const char *s)
{
	uint64_t u;
	unsigned int i;
	i = 8;
	u = 0;
	while (i && *s)
	{
		u = (u << 8) | *(unsigned char *) s;
		++s;
		--i;
	}
	return u;
}

int main(int argc, const char *argv[])
{
	wav_load_t *wl;
	const char *input;
	const char *output;
	uint32_t channels;
	uint32_t sampfre;
	int i;
	input = output = NULL;
	channels = 1;
	sampfre = 96000;
	if (argc > 1)
	{
		i = 1;
		while (i < argc)
		{
			if (*argv[i] == '-')
			{
				switch (argv_s2u(argv[i] + 1))
				{
					case 'o':
						if (++i >= argc) goto Err;
						if (output) goto Err;
						output = argv[i];
						break;
					case 'c':
						if (++i >= argc) goto Err;
						channels = atoi(argv[i]);
						break;
					case 's':
						if (++i >= argc) goto Err;
						sampfre = atoi(argv[i]);
						break;
					case 'h':
					case (((uint64_t) '-' << 32) | (uint64_t) 'help'):
						goto label_help;
					default:
						goto Err;
				}
			}
			else
			{
				if (input) goto Err;
				input = argv[i];
			}
			++i;
		}
		if (!input || !output) goto Err;
		i = -1;
		wl = pcms32le_load(input, channels, sampfre);
		if (wl)
		{
			i = wav_save_load(output, wl);
			if (i) printf("save wav[%s] fail\n", output);
			wav_free(wl);
		}
		else printf("don't load pcms32le file [%s]\n", input);
	}
	else
	{
		label_help:
		printf("%s <input:pcms32le> <-o output:wav> [-c channels (1)] [-s sampfre (96000)]\n", argv[0]);
		i = 0;
	}
	return i;
	Err:
	printf("please use '%s -h/--help'\n", argv[0]);
	return -1;
}
