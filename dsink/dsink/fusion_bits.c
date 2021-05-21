#include "inner.h"

uintptr_t dsink_inner_fusion_bits(register const dsink_fusion_t *restrict fusion, register uintptr_t n, uintptr_t *restrict rlength)
{
	register uintptr_t bits, length;
	bits = length = 0;
	while (n)
	{
		--n;
		bits += fusion[n].bits * fusion[n].length;
		length += fusion[n].length;
	}
	if (rlength) *rlength = length;
	return bits;
}
