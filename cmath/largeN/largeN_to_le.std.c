#include "largeN.h"
#include <memory.h>

largeN_store_s* largeN_to_le(largeN_store_s *restrict dst, const largeN_s *restrict src)
{
	unit_t i, n;
	if ((n = dst->n) > src->n)
	{
		n = src->n;
		memset(dst->le + n, 0, (dst->n - n) * sizeof(unit_t));
	}
	for (i = 0; i < n; ++i)
		dst->le[i] = order_n2le(src->le[i]);
	return dst;
}
