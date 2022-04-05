#include "largeN.h"
#include <memory.h>

largeN_s* largeN_set(largeN_s *restrict dst, const largeN_s *restrict src)
{
	unit_t n;
	if ((n = dst->n) > src->n)
	{
		n = src->n;
		memset(dst->le + n, 0, (dst->n - n) * sizeof(unit_t));
	}
	memcpy(dst->le, src->le, n * sizeof(unit_t));
	return dst;
}
