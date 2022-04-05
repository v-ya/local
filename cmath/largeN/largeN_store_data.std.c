#include "largeN.h"
#include <memory.h>

uint8_t* largeN_store_data(const largeN_store_s *restrict src, uintptr_t *restrict rsize)
{
	if (rsize) *rsize = src->n * sizeof(unit_t);
	return (uint8_t *) src->le;
}
