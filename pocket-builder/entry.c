#include <pocket/pocket.h>
#include <pocket/pocket-saver.h>
#include <pocket/pocket-verify.h>
#include <stdio.h>
#include <time.h>
#include "entry_arg.h"
#include "script.h"
#include "script_parse.h"
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

static int do_script(const char *restrict input, const char *restrict output)
{
	int ret;
	buffer_t *buffer;
	ret = -1;
	if ((buffer = buffer_alloc()))
	{
		if (load_text(buffer, input))
		{
			script_t *script;
			pocket_saver_s *saver;
			pocket_verify_s *verify;
			script = script_alloc();
			saver = pocket_saver_alloc();
			verify = pocket_verify_default();
			if (saver && verify && saver_set_time(saver))
			{
				const char *restrict rpos;
				rpos = script_build(script, saver, (const char *) buffer->data);
				if (!rpos)
				{
					if (pocket_saver_save(saver, output, verify))
						ret = 0;
					else printf("save pocket[%s] fail\n", output);
				}
				else script_error((const char *) buffer->data, rpos);
			}
			else printf("build env fail\n");
			if (script) script_free(script);
			if (saver) refer_free(saver);
			if (verify) refer_free(verify);
		}
		else printf("script[%s] load fail\n", input);
		buffer_free(buffer);
	}
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
			if (args.output) return do_script(args.input, args.output);
			else return do_pocket(args.input);
		}
	}
	args_help(*argv);
	return (ret > 0)?0:-1;
}
