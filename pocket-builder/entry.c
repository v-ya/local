#include <pocket/pocket.h>
#include <pocket/pocket-saver.h>
#include <pocket/pocket-verify.h>
#include <fsys.h>
#include <stdio.h>
#include <time.h>
#include "entry_arg.h"
#include "script.h"
#include "pocket_dump.h"

static pocket_saver_s* saver_set_time(pocket_saver_s *restrict saver)
{
	char st[64];
	time_t this;
	struct tm tm;
	time(&this);
	localtime_r(&this, &tm);
	sprintf(st, "%04d-%02d-%02d %02d:%02d:%02d",
		tm.tm_year + 1900,
		tm.tm_mon + 1,
		tm.tm_mday,
		tm.tm_hour,
		tm.tm_min,
		tm.tm_sec);
	return pocket_saver_set_time(saver, st);
}

static refer_nstring_t load_file(const char *restrict path)
{
	fsys_file_s *restrict fp;
	refer_nstring_t data;
	data = NULL;
	fp = fsys_file_alloc(path, fsys_file_flag_read);
	if (fp)
	{
		data = fsys_file_aread_all(fp);
		refer_free(fp);
	}
	return data;
}

static const uint8_t* save_file(const char *restrict path, const uint8_t *restrict data, uintptr_t size)
{
	fsys_file_s *restrict fp;
	uintptr_t rsize;
	const uint8_t *r;
	r = NULL;
	fp = fsys_file_alloc(path, fsys_file_flag_write | fsys_file_flag_create | fsys_file_flag_truncate);
	if (fp)
	{
		if (fsys_file_write(fp, data, size, &rsize) && rsize == size)
			r = data;
		refer_free(fp);
	}
	return r;
}

static void script_error(register const char *restrict start, register const char *restrict rpos)
{
	uintptr_t line, col;
	line = col = 0;
	while (start < rpos)
	{
		++col;
		if (*start++ == '\n')
		{
			col = 0;
			++line;
		}
	}
	printf("script parse error at [%lu:%lu]\n", line + 1, col + 1);
}

static int do_script(const args_t *restrict args)
{
	int ret;
	refer_nstring_t text;
	ret = -1;
	if ((text = load_file(args->input)))
	{
		script_t *script;
		pocket_saver_s *saver;
		pocket_verify_s *verify;
		script = script_alloc();
		saver = pocket_saver_alloc();
		verify = pocket_verify_default();
		if (script && saver && verify && saver_set_time(saver))
		{
			const char *restrict rpos;
			const char *restrict linker;
			if (args->kiya_pocket_path)
			{
				if (!script_kiya_enable(script, args->kiya_xmsize, verify))
				{
					printf("kiya enable fail\n");
					goto label_fail;
				}
				if (!script_kiya_load(script, args->kiya_pocket_path))
				{
					printf("kiya pocket[%s] load fail\n", args->kiya_pocket_path);
					goto label_fail;
				}
			}
			rpos = script_build(script, saver, text->string, &linker);
			if (!rpos)
			{
				uint8_t *data;
				uint64_t size;
				data = pocket_saver_build(saver, &size, verify);
				if (data)
				{
					if (linker)
					{
						pocket_s *pocket;
						pocket = pocket_alloc(data, size, verify);
						if (pocket)
						{
							rpos = script_link(pocket, linker);
							refer_free(pocket);
							if (!rpos)
							{
								if (pocket_build_verify(verify, data, size))
									linker = NULL;
								else printf("rebuild pocket verify fail\n");
							}
							else script_error(text->string, rpos);
						}
					}
					if (!linker)
					{
						if (save_file(args->output, data, (size_t) size))
							ret = 0;
						else printf("save pocket[%s] fail\n", args->output);
						pocket_saver_build_free(data);
					}
				}
				else printf("build pocket fail\n");
			}
			else script_error(text->string, rpos);
		}
		else printf("build env fail\n");
		label_fail:
		if (script) script_free(script);
		if (saver) refer_free(saver);
		if (verify) refer_free(verify);
		refer_free(text);
	}
	else printf("script[%s] load fail\n", args->input);
	return ret;
}

static int do_pocket(const char *restrict input)
{
	int ret;
	pocket_verify_s *verify;
	ret = -1;
	verify = pocket_verify_default();
	if (verify)
	{
		pocket_s *restrict pocket;
		pocket = pocket_load(input, verify);
		if (pocket)
		{
			pocket_dump(pocket, ~(dump_bits_t) 0);
			refer_free(pocket);
			ret = 0;
		}
		else printf("pocket[%s] load fail, maybe verify fail\n", input);
		refer_free(verify);
	}
	else printf("default verify alloc fail\n");
	return ret;
}

int main(int argc, const char *argv[])
{
	args_t args;
	int ret;
	if (!(ret = args_init(&args, argc, argv)))
	{
		if (args.input)
		{
			if (args.output) return do_script(&args);
			else return do_pocket(args.input);
		}
	}
	args_help(*argv);
	return (ret > 0)?0:-1;
}
