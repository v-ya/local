#include "largeN.h"

largeN_s* largeN_alloc(uintptr_t bits)
{
	largeN_s *restrict r;
	if ((bits = bits2unit(bits)) &&
		(r = (largeN_s *) refer_alloc(sizeof(largeN_s) + sizeof(unit_t) * bits)))
	{
		r->n = bits;
		return r;
	}
	return NULL;
}
