#include "inner.h"

void dsink_inner_fusion_limit_bits(register dsink_fusion_t *restrict fusion, register uintptr_t n, register uint32_t max_bits)
{
	while (n)
	{
		--n;
		if (fusion[n].bits > max_bits)
			fusion[n].bits = max_bits;
	}
}
