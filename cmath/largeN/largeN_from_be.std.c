#include "largeN.h"
#include <memory.h>

largeN_s* largeN_from_be(largeN_s *restrict dst, const largeN_store_s *restrict src)
{
	unit_t i, n, m;
	if ((n = dst->n) > (m = src->n))
	{
		memset(dst->le + m, 0, (n - m) * sizeof(unit_t));
		n = m;
	}
	m -= 1;
	for (i = 0; i < n; ++i)
		dst->le[i] = order_n2be(src->le[m - i]);
	return largeN_measure_effective(dst);
}
