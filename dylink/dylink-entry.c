#include "dylink.h"
#include "elfobj.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static uint8_t* file_load(const char *path, size_t *psize)
{
	uint8_t *r;
	size_t size;
	FILE *fp;
	r = NULL;
	size = 0;
	fp = fopen(path, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		if (size)
		{
			r = (uint8_t *) malloc(size);
			if (r)
			{
				fseek(fp, 0, SEEK_SET);
				if (fread(r, 1, size, fp) != size)
				{
					free(r);
					r = NULL;
				}
			}
		}
		fclose(fp);
	}
	if (psize) *psize = size;
	return r;
}

static int file_save(const char *path, uint8_t *data, size_t size)
{
	FILE *fp;
	int r;
	r = -1;
	fp = fopen(path, "wb");
	if (fp)
	{
		if (fwrite(data, 1, size, fp) == size)
			r = 0;
		fclose(fp);
	}
	return r;
}

int main(int argc, const char *argv[])
{
	const char *output, *input;
	elf64obj_s *e;
	uint8_t *data;
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
			e = elf64obj_load(input);
			if (!e) printf("load elf64[%s] fail\n", input);
			else
			{
				data = elf64obj_build_dylink(e, &size);
				if (!data) printf("build elf64[%s] to dylink fail\n", input);
				else
				{
					if (!file_save(output, data, size)) i = 0;
					else printf("save dylink[%s](size = %lu) fail\n", output, size);
					free(data);
				}
				elf64obj_free(e);
			}
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
					e = elf64obj_load(input);
					if (!e) printf("load elf64[%s] fail\n", input);
					else
					{
						elf64obj_dump_session(e);
						elf64obj_dump_symtab(e);
						elf64obj_dump_rela(e);
						elf64obj_free(e);
						i = 0;
					}
				}
				else if (size > sizeof(dylink_header_s) && data[0] == 'd' && data[1] == 'y' && data[2] == 'l' && data[3] == '.')
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
