#include "dylink.h"
#include "elf2dylink.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fsys.h>

static uint64_t argv_s2u(const char *restrict s)
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

static uint8_t* file_load(const char *restrict path, uintptr_t *restrict rsize)
{
	fsys_file_s *restrict fp;
	uint64_t size;
	uint8_t *restrict r;
	uintptr_t n;
	r = NULL;
	n = 0;
	if ((fp = fsys_file_alloc(path, fsys_file_flag_read)))
	{
		if (fsys_file_attr_size(fp, &size) && size)
		{
			if ((r = (uint8_t *) malloc((uintptr_t) size)))
			{
				if (!fsys_file_read(fp, r, (uintptr_t) size, &n) ||
					n != (uintptr_t) size)
				{
					free(r);
					r = NULL;
				}
			}
		}
		refer_free(fp);
	}
	if (rsize) *rsize = n;
	return r;
}

static int file_save(const char *path, const uint8_t *restrict data, uintptr_t size)
{
	fsys_file_s *restrict fp;
	uintptr_t rn;
	int r;
	r = -1;
	if ((fp = fsys_file_alloc(path, fsys_file_flag_write | fsys_file_flag_create | fsys_file_flag_truncate)))
	{
		if (fsys_file_write(fp, data, size, &rn) && rn == size)
			r = 0;
		refer_free(fp);
	}
	return r;
}

int main(int argc, const char *argv[])
{
	const char *restrict output, *restrict input;
	elf2dylink_s *restrict e2d;
	uint8_t *restrict data;
	size_t size;
	int i;
	if (argc > 1)
	{
		input = output = NULL;
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
		if (!input) goto Err;
		i = -1;
		if (output)
		{
			// elf => dylink
			if ((e2d = elf2dylink_load_by_path(input)))
			{
				if ((data = elf2dylink_build_dylink(e2d, &size)))
				{
					if (!file_save(output, data, size)) i = 0;
					else printf("save dylink[%s](size = %lu) fail\n", output, size);
				}
				else printf("build elf64[%s] to dylink fail\n", input);
				refer_free(e2d);
			}
			else printf("load elf64[%s] fail\n", input);
		}
		else
		{
			data = file_load(input, &size);
			if (data)
			{
				// info
				if (size > 4 && data[0] == 0x7f && data[1] == 'E' && data[2] == 'L' && data[3] == 'F')
				{
					// elf
					elf2dylink_s *restrict e2d;
					if ((e2d = elf2dylink_load_by_memory(data, size)))
					{
						elf2dylink_dump_elf(e2d);
						refer_free(e2d);
					}
					else printf("load elf64[%s] fail\n", input);
				}
				else if (size > sizeof(dylink_header_t) && data[0] == 'd' && data[1] == 'y' && data[2] == 'l' && data[3] == '.')
				{
					// dylink
					dylink_dump(data, size);
					i = 0;
				}
				else printf("unknow input[%s] type(elf, dylink)\n", input);
				free(data);
			}
			else printf("load file[%s] fail\n", input);
		}
	}
	else
	{
		label_help:
		printf("%s <input> [-o output]\n", argv[0]);
		i = 0;
	}
	return i;
	Err:
	printf("please use '%s -h/--help'\n", argv[0]);
	return -1;
}
