#include "largeN.h"
#include <memory.h>

largeN_s* largeN_zero(largeN_s *restrict dst)
{
	memset(dst->le, 0, dst->n * sizeof(unit_t));
	dst->e = 0;
	return dst;
}
