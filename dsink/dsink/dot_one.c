#include "inner.h"

int64_t dsink_inner_dot_one(register const int32_t *restrict data, register uintptr_t n)
{
	register int64_t r = 0;
	while (n)
		r += data[--n];
	return r;
}
