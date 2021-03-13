#include "dtv.h"
#include <stdint.h>
#include <stdlib.h>

struct gnu_linux_amd64_dtv_pointer_t {
	void *v;
	void *f;
};

union gnu_linux_amd64_dtv_t {
	uintptr_t n;
	struct gnu_linux_amd64_dtv_pointer_t p;
};

void* gnu_linux_amd64_dtv_alloc(void)
{
	register union gnu_linux_amd64_dtv_t *dtv;
	dtv = (union gnu_linux_amd64_dtv_t *) calloc(2, sizeof(union gnu_linux_amd64_dtv_t));
	return dtv + !!dtv;
}

void gnu_linux_amd64_dtv_free(void *dtv)
{
	union gnu_linux_amd64_dtv_t *p;
	uintptr_t n;
	p = (union gnu_linux_amd64_dtv_t *) dtv + 1;
	n = p[-1].n;
	while (n)
	{
		--n;
		if (p->p.f)
			free(p->p.f);
		++p;
	}
	free((union gnu_linux_amd64_dtv_t *) dtv - 1);
}

