#include "largeN.h"

uintptr_t largeN_bits_effective(const largeN_s *restrict src)
{
	uintptr_t ebits;
	if ((ebits = src->e << bits2unit_shift))
		ebits -= unit_clz(src->le[src->e - 1]);
	return ebits;
}
