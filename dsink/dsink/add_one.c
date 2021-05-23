#include "inner.h"

void dsink_inner_add_one(register int32_t *restrict data, register uintptr_t n, register int32_t amplitude)
{
	while (n)
		data[--n] += amplitude;
}
