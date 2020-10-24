#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <hashmap.h>
#include <args.h>
#include "rhash.h"

typedef struct args_t {
	const char *input;
	uint32_t mbits;
	uint32_t nblock;
	uint32_t cache;
	uint32_t strmode;
} args_t;

static args_deal_func(__, args_t*)
{
	if (*index)
	{
		if (!value) return -1;
		pri->input = value;
		if (command) ++*index;
	}
	return 0;
}

static args_deal_func(_m, args_t*)
{
	if (!value) return -1;
	pri->mbits = atoi(value);
	++*index;
	return 0;
}

static args_deal_func(_n, args_t*)
{
	if (!value) return -1;
	pri->nblock = atoi(value);
	++*index;
	return 0;
}

static args_deal_func(_c, args_t*)
{
	uint32_t v;
	if (!value) return -1;
	if ((v = atoi(value)))
		pri->cache = v;
	++*index;
	return 0;
}

static args_deal_func(_s, args_t*)
{
	pri->strmode = 1;
	return 0;
}

static args_deal_func(_h, args_t*)
{
	return 1;
}

static int args(args_t *restrict pri, int argc, const char *argv[])
{
	hashmap_t a;
	int r;
	r = -1;
	if (!hashmap_init(&a)) goto Err;
	if (!args_set_command(&a, "--", (args_deal_f) __)) goto Err;
	if (!args_set_command(&a, "-m", (args_deal_f) _m)) goto Err;
	if (!args_set_command(&a, "-n", (args_deal_f) _n)) goto Err;
	if (!args_set_command(&a, "-c", (args_deal_f) _c)) goto Err;
	if (!args_set_command(&a, "-s", (args_deal_f) _s)) goto Err;
	if (!args_set_command(&a, "-h", (args_deal_f) _h)) goto Err;
	if (!args_set_command(&a, "--help", (args_deal_f) _h)) goto Err;
	r = args_deal(argc, argv, &a, (args_deal_f) __, pri);
	Err:
	hashmap_uini(&a);
	return r;
}

static void args_init(args_t *restrict pri, const uint8_t *restrict exec)
{
	static uint8_t schar[256] = {
		['0' ... '9'] = 1,
		['A' ... 'Z'] = 2,
		['a' ... 'z'] = 2,
	};
	pri->input = NULL;
	pri->mbits = 64;
	pri->nblock = 4;
	pri->cache = 16u << 10;
	pri->strmode = 0;
	do {
		if (schar[*exec])
		{
			if (exec[0] == 'm' && schar[exec[1]] == 1)
				pri->mbits = atoi((char *) exec + 1);
			else if (exec[0] == 'n' && schar[exec[1]] == 1)
				pri->nblock = atoi((char *) exec + 1);
			while (schar[*exec]) ++exec;
		}
		else while (*exec && !schar[*exec]) ++exec;
	} while (*exec);
}

static void args_help(args_t *restrict pri, const char *restrict exec)
{
	printf("%s <input:stdin|file|string> [-m mbits (%u == 64, 32)] [-n nblock (%u > 0)] [-c (%u > 0)] [-s (string mode)]\n", exec, pri->mbits, pri->nblock, pri->cache);
}

static void data_dump(const uint8_t *data, size_t size, const args_t *restrict args)
{
	while (size)
	{
		printf("%02x", *data++);
		--size;
	}
	if (args->strmode && args->input) printf("  \"%s\"\n", args->input);
	else printf("  %s\n", args->input?args->input:"-");
}

int rhash_args_send_64(rhash64_s *restrict rh64, const args_t *restrict args)
{
	uint64_t *restrict data;
	size_t n, nb;
	uint64_t res;
	if (args->strmode && args->input)
	{
		data = (uint64_t *) args->input;
		n = strlen(args->input);
		if ((nb = n >> 3))
			rhash64_send(rh64, data, nb);
		if ((n &= 7))
		{
			res = 0;
			memcpy(&res, data + nb, n);
			rhash64_send(rh64, &res, 1);
		}
		return 0;
	}
	else
	{
		size_t size;
		ssize_t length;
		int fd;
		size = args->cache & ~7;
		data = (uint64_t *) malloc(size);
		if (data)
		{
			if (args->input) fd = open(args->input, O_RDONLY);
			else fd = STDIN_FILENO;
			if (~fd)
			{
				n = 0;
				do {
					length = read(fd, (uint8_t *) data + n, size - n);
					if (length < 0) break;
					if ((nb = (n += (size_t) length) >> 3))
					{
						rhash64_send(rh64, data, nb);
						if ((n &= 7)) memmove(data, data + nb, n);
					}
				} while (length);
				if (n)
				{
					
					res = 0;
					memcpy(&res, data + nb, n);
					rhash64_send(rh64, &res, 1);
				}
				if (args->input) close(fd);
			}
			free(data);
		}
		if (data && ~fd)
			return 0;
	}
	return 1;
}

int rhash(const args_t *restrict r)
{
	int ret = -1;
	if (r->nblock)
	{
		if (r->mbits == 64)
		{
			rhash64_s *restrict rh64;
			rh64 = rhash64_alloc(NULL, r->nblock, 0);
			if (rh64)
			{
				if (!(ret = rhash_args_send_64(rh64, r)))
					data_dump((uint8_t *) rhash64_final(rh64), sizeof(uint64_t) * r->nblock, r);
				refer_free(rh64);
			}
		}
		else if (r->mbits == 32)
		{
			printf("'-m 32' is currently not supported\n");
			ret = 1;
		}
	}
	return ret;
}

int main(int argc, const char *argv[])
{
	args_t r;
	int ret;
	args_init(&r, (const uint8_t *) argv[0]);
	ret = args(&r, argc, argv);
	if (ret > 0)
	{
		args_help(&r, argv[0]);
		ret = 0;
	}
	else
	{
		if (ret < 0) goto Err;
		ret = rhash(&r);
		if (ret < 0) goto Err;
	}
	return ret;
	Err:
	printf("please use '%s -h/--help'\n", argv[0]);
	return -1;
}
