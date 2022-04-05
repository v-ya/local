#include "largeN.h"
#include <memory.h>

largeN_store_s* largeN_to_be(largeN_store_s *restrict dst, const largeN_s *restrict src)
{
	unit_t i, n, m;
	if ((m = n = dst->n) > src->n)
	{
		n = src->n;
		memset(dst->le, 0, (m - n) * sizeof(unit_t));
	}
	m -= 1;
	for (i = 0; i < n; ++i)
		dst->le[m - i] = order_n2be(src->le[i]);
	return dst;
}
