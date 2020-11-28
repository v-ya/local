#include "../largeN.h"
#include <stdlib.h>

largeN_t* largeN_alloc(register uint64_t n)
{
	register largeN_t *restrict r;
	r = (largeN_t *) malloc(sizeof(uint64_t) * (n + 1));
	if (r) r->n = n;
	return r;
}

largeN_t* largeN_alloz(register uint64_t n)
{
	register largeN_t *restrict r;
	r = (largeN_t *) calloc(n + 1, sizeof(uint64_t));
	if (r) r->n = n;
	return r;
}

void largeN_free(register largeN_t *ln)
{
	free(ln);
}
