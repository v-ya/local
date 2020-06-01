#include "phoneme_script.h"
#include <wav.h>
#include <args.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "core/core.h"

static void _help(const char *exec, const char *type)
{
	printf(
		"%s [control] <-o/--output output> <[--] intput>\n"
		"\t*" "/--                             [input phoneme script]\n"
		"\t*" "-o/--output                     [output wav]\n"
		"\t " "-cp/--core-path                 [core-path/*: core.dylink]\n"
		"\t " "-pp/--package-path              [package-path/*: package.json]\n"
		"\t " "-bt/--base-time                 [float > 0]\n"
		"\t " "-bv/--base-volume               [float > 0]\n"
		"\t " "-bf/--base-freline              [float > 0]\n"
		"\t " "-bfs/--base-frestep             [float != 0]\n"
		"\t " "-st/--space-time                [float]\n"
		"\t " "-sm/-sdmax/--stage-details-max  [integer > 0]\n"
		"\t " "-dm/-dmax/--details-max         [integer > 0]\n"
		"\t " "-sf/--sampfre                   [integer > 0]\n"
		"\t " "-xm/--xmsize                    [integer > 0]\n"
		"\t " "-h/--help\n"
		, exec
	);
}

typedef struct args_t {
	const char *input;
	const char *output;
	const char *core_path;
	const char *package_path;
	double base_time;
	double base_volume;
	double base_fre_line;
	double base_fre_step;
	double space_time;
	uint32_t space_time_used;
	uint32_t sdmax;
	uint32_t dmax;
	uint32_t sampfre;
	size_t xmsize;
} args_t;

static args_deal_func(__, args_t*)
{
	pri->input = value;
	if (command) ++*index;
	return 0;
}

static args_deal_func(_o, args_t*)
{
	pri->output = value;
	++*index;
	return 0;
}

static args_deal_func(_cp, args_t*)
{
	pri->core_path = value;
	++*index;
	return 0;
}

static args_deal_func(_pp, args_t*)
{
	pri->package_path = value;
	++*index;
	return 0;
}

static args_deal_func(_bt, args_t*)
{
	if (!value || (pri->base_time = atof(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_bv, args_t*)
{
	if (!value || (pri->base_volume = atof(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_bf, args_t*)
{
	if (!value || (pri->base_fre_line = atof(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_bfs, args_t*)
{
	if (!value || !(pri->base_fre_step = atof(value))) return -1;
	++*index;
	return 0;
}

static args_deal_func(_st, args_t*)
{
	if (!value) return -1;
	pri->space_time = atof(value);
	pri->space_time_used = 1;
	++*index;
	return 0;
}

static args_deal_func(_sm, args_t*)
{
	if (!value || (int)(pri->sdmax = atoi(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_dm, args_t*)
{
	if (!value || (int)(pri->dmax = atoi(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_sf, args_t*)
{
	if (!value || (int)(pri->sampfre = atoi(value)) <= 0) return -1;
	++*index;
	return 0;
}

static args_deal_func(_xm, args_t*)
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

static args_deal_func(_h, args_t*)
{
	_help(argv[0], value);
	return 1;
}

static int args_init(args_t *pri, int argc, const char *argv[])
{
	hashmap_t args;
	int r;
	memset(pri, 0, sizeof(args_t));
	pri->sampfre = 96000;
	pri->xmsize = 1 << 20;
	r = -1;
	if (!hashmap_init(&args)) goto Err;
	if (!args_set_command(&args, "--", (args_deal_f) __)) goto Err;
	if (!args_set_command(&args, "-o", (args_deal_f) _o)) goto Err;
	if (!args_set_command(&args, "--output", (args_deal_f) _o)) goto Err;
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
	if (!args_set_command(&args, "-sf", (args_deal_f) _sf)) goto Err;
	if (!args_set_command(&args, "--sampfre", (args_deal_f) _sf)) goto Err;
	if (!args_set_command(&args, "-xm", (args_deal_f) _xm)) goto Err;
	if (!args_set_command(&args, "--xmsize", (args_deal_f) _xm)) goto Err;
	if (!args_set_command(&args, "-h", (args_deal_f) _h)) goto Err;
	if (!args_set_command(&args, "--help", (args_deal_f) _h)) goto Err;
	r = args_deal(argc, argv, &args, (args_deal_f) __, pri);
	Err:
	hashmap_uini(&args);
	return r;
}

static int args_check(args_t *pri)
{
	if (!pri->input || !pri->output || !pri->xmsize)
		return -1;
	if (pri->xmsize < 4096) pri->xmsize = 4096;
	return 0;
}

static int sysfunc(dylink_pool_t *dyp, void *pri)
{
	int r;
	r = 0;
	#define set_func(_f)  r += !!dylink_pool_set_func(dyp, #_f, _f)
	// stdio.h
	set_func(fclose);
	// set_func(clearerr);
	// set_func(feof);
	// set_func(ferror);
	// set_func(fflush);
	// set_func(fgetpos);
	set_func(fopen);
	set_func(fread);
	// set_func(freopen);
	set_func(fseek);
	// set_func(fsetpos);
	set_func(ftell);
	// set_func(fwrite);
	// set_func(remove);
	// set_func(rename);
	// set_func(rewind);
	// set_func(setbuf);
	// set_func(setvbuf);
	// set_func(tmpfile);
	// set_func(tmpnam);
	// set_func(fprintf);
	set_func(printf);
	set_func(sprintf);
	// set_func(vfprintf);
	set_func(vprintf);
	set_func(vsprintf);
	// set_func(fscanf);
	// set_func(scanf);
	// set_func(sscanf);
	// set_func(fgetc);
	// set_func(fgets);
	// set_func(fputc);
	// set_func(fputs);
	// set_func(getc);
	// set_func(getchar);
	// set_func(putc);
	set_func(putchar);
	// set_func(puts);
	// set_func(ungetc);
	// set_func(perror);
	set_func(snprintf);
	// ==========
	// stdlib.h
	set_func(atof);
	set_func(atoi);
	set_func(atol);
	set_func(strtod);
	set_func(strtol);
	set_func(strtoul);
	set_func(calloc);
	set_func(free);
	set_func(malloc);
	set_func(realloc);
	// set_func(abort);
	// set_func(atexit);
	// set_func(exit);
	// set_func(getenv);
	// set_func(system);
	// set_func(bsearch);
	// set_func(qsort);
	set_func(abs);
	set_func(div);
	set_func(labs);
	set_func(ldiv);
	set_func(rand);
	set_func(srand);
	// set_func(mblen);
	// set_func(mbstowcs);
	// set_func(mbtowc);
	// set_func(wcstombs);
	// set_func(wctomb);
	// ==========
	// string.h
	set_func(memchr);
	set_func(memcmp);
	set_func(memcpy);
	set_func(memmove);
	set_func(memset);
	set_func(strcat);
	set_func(strncat);
	set_func(strchr);
	set_func(strcmp);
	set_func(strncmp);
	set_func(strcoll);
	set_func(strcpy);
	set_func(strncpy);
	set_func(strcspn);
	// set_func(strerror);
	set_func(strlen);
	set_func(strpbrk);
	set_func(strrchr);
	set_func(strspn);
	set_func(strstr);
	set_func(strtok);
	set_func(strxfrm);
	// ==========
	// math.h
	set_func(acos);
	set_func(asin);
	set_func(atan);
	set_func(atan2);
	set_func(cos);
	set_func(cosh);
	set_func(sin);
	set_func(sinh);
	set_func(tan);
	set_func(tanh);
	set_func(exp);
	set_func(frexp);
	set_func(ldexp);
	set_func(log);
	set_func(log10);
	set_func(modf);
	set_func(pow);
	set_func(sqrt);
	set_func(ceil);
	set_func(fabs);
	set_func(floor);
	set_func(fmod);
	#undef set_func
	if (!r)
	{
		r = dylink_pool_load(dyp, dy_core, dy_core_size);
		#define unset_func(_f)  dylink_pool_delete_symbol(dyp, #_f)
		unset_func(fclose);
		unset_func(fopen);
		unset_func(fread);
		unset_func(fseek);
		unset_func(ftell);
		#undef unset_func
	}
	return r;
}

int main(int argc, const char *argv[])
{
	phoneme_script_s *ps;
	phoneme_buffer_s *pb;
	args_t args;
	int r;
	r = 0;
	if (argc == 1) _help(argv[0], NULL);
	else
	{
		r = args_init(&args, argc, argv);
		if (r > 0) r = 0;
		else if (!r && !(r = args_check(&args)))
		{
			r = -1;
			ps = phoneme_script_alloc(args.xmsize, (phoneme_script_sysfunc_f) sysfunc, NULL);
			if (ps)
			{
				if ((!args.core_path || phoneme_script_set_core_path(ps, args.core_path)) &&
					(!args.package_path || phoneme_script_set_package_path(ps, args.package_path)))
				{
					if (args.base_time) ps->base_time = args.base_time;
					if (args.base_volume) ps->base_volume = args.base_volume;
					if (args.base_fre_line) ps->base_fre_line = args.base_fre_line;
					if (args.base_fre_step) ps->base_fre_step = args.base_fre_step;
					if (args.space_time_used) ps->space_time = args.space_time;
					if (args.sdmax) ps->sdmax = args.sdmax;
					if (args.dmax) ps->dmax = args.dmax;
					pb = phoneme_buffer_alloc(args.sampfre, 0);
					if (pb)
					{
						if (phoneme_script_load(ps, args.input, pb))
						{
							if (!wav_save_double(args.output, &pb->buffer, pb->frames, 1, pb->sampfre))
								r = 0;
						}
						else printf("phoneme_script_load fail ...\n");
						refer_free(pb);
					}
				}
				refer_free(ps);
			}
		}
	}
	return r;
}

