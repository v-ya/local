#define _GNU_SOURCE
#include "phoneme_resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "core/core.h"

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
	set_func(putc);
	set_func(putchar);
	set_func(puts);
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
	// double
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
	set_func(sincos);
	set_func(exp);
	set_func(exp2);
	set_func(exp10);
	set_func(frexp);
	set_func(ldexp);
	set_func(log);
	set_func(log2);
	set_func(log10);
	set_func(modf);
	set_func(pow);
	set_func(sqrt);
	set_func(ceil);
	set_func(fabs);
	set_func(floor);
	set_func(fmod);
	// float
	set_func(acosf);
	set_func(asinf);
	set_func(atanf);
	set_func(atan2f);
	set_func(cosf);
	set_func(coshf);
	set_func(sinf);
	set_func(sinhf);
	set_func(tanf);
	set_func(tanhf);
	set_func(sincosf);
	set_func(expf);
	set_func(exp2f);
	set_func(exp10f);
	set_func(frexpf);
	set_func(ldexpf);
	set_func(logf);
	set_func(log2f);
	set_func(log10f);
	set_func(modff);
	set_func(powf);
	set_func(sqrtf);
	set_func(ceilf);
	set_func(fabsf);
	set_func(floorf);
	set_func(fmodf);
	#undef set_func
	r += !!dylink_pool_set_func(dyp, "dypool.symbol", dylink_pool_get_symbol);
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

static phoneme_script_s* sysset(phoneme_script_s *restrict ps)
{
	phoneme_script_s* (*init)(phoneme_script_s *restrict);
	init = dylink_pool_get_symbol(ps->phoneme_pool->dypool, "this.init", NULL);
	if (init) return init(ps);
	return NULL;
}

phoneme_script_s* phoneme_script_create_default(size_t xmsize, mlog_s *restrict mlog)
{
	phoneme_script_s *restrict r;
	r = phoneme_script_alloc(xmsize, mlog, sysfunc, NULL);
	if (r)
	{
		if (sysset(r))
			return r;
		refer_free(r);
	}
	return NULL;
}

