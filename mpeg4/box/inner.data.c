#include "inner.data.h"
#include <memory.h>

void* mpeg4$define(inner, data, get)(void *restrict r, size_t rsize, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	if (*size >= rsize)
	{
		memcpy(r, *data, rsize);
		*data += rsize;
		*size -= rsize;
		return r;
	}
	return NULL;
}

uint32_t* mpeg4$define(inner, uint32_t, get)(uint32_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	if (*size >= sizeof(*r))
	{
		memcpy(r, *data, sizeof(*r));
		*r = mpeg4_n32(*r);
		*data += sizeof(*r);
		*size -= sizeof(*r);
		return r;
	}
	return NULL;
}
