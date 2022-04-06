#include "largeN.h"

uintptr_t largeN_bits_allow(const largeN_s *restrict src)
{
	return src->n << bits2unit_shift;
}
