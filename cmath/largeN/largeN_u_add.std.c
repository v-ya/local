#include "largeN.h"

uintptr_t largeN_u_add(largeN_s *restrict dst, uintptr_t u)
{
	full_t z;
	uintptr_t i, n;
	n = dst->n;
	for (i = 0; u && i < n; ++i)
	{
		z = (full_t) dst->le[i] + u;
		dst->le[i] = (unit_t) z;
		u = (unit_t) (z >> unit_bits);
	}
	if (u) largeN_measure_effective(dst);
	else if (dst->e < i) dst->e = i;
	return u;
}
