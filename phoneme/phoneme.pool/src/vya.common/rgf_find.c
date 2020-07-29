#include <phoneme/phoneme.h>
#include <string.h>
#include <alloca.h>
#include "../pthis.h"

static dyl_used void* rgf_find(const char *restrict sym, void **restrict arg2pri)
{
	void *r;
	if (!strncmp(sym, "vya.rg.", 7))
	{
		r = pthis_symbol(sym);
		if (r)
		{
			if (arg2pri)
			{
				size_t n;
				char *restrict s;
				n = strlen(sym);
				s = (char *) alloca(n + 5);
				if (!s) goto Err;
				memcpy(s, sym, n);
				s[n++] = '.';
				s[n++] = 'a';
				s[n++] = 'r';
				s[n++] = 'g';
				s[n] = 0;
				*arg2pri = pthis_symbol(s);
			}
			return r;
		}
	}
	Err:
	if (arg2pri) *arg2pri = NULL;
	return NULL;
}
dyl_export(rgf_find, vya.common.rgf_find);

