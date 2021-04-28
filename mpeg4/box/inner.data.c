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

uint8_t* mpeg4$define(inner, data, set)(uint8_t *restrict data, void *restrict v, size_t vsize)
{
	memcpy(data, v, vsize);
	return data + vsize;
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

uint8_t* mpeg4$define(inner, uint32_t, set)(uint8_t *restrict data, uint32_t v)
{
	v = mpeg4_n32(v);
	memcpy(data, &v, sizeof(v));
	return data + sizeof(v);
}

int16_t mpeg4$define(inner, f2i, int16_t)(double v)
{
	if (v >= 0)
	{
		if (v < 32767)
			return (int16_t) (v + 0.5);
		else return 32767;
	}
	else
	{
		if (v < -32768)
			return (int16_t) (v - 0.5);
		else return -32768;
	}
}

uint16_t mpeg4$define(inner, f2i, uint16_t)(double v)
{
	if (v >= 0)
	{
		if (v < 65535)
			return (uint16_t) (v + 0.5);
		else return 65535;
	}
	else return 0;
}

int32_t mpeg4$define(inner, f2i, int32_t)(double v)
{
	if (v >= 0)
	{
		if (v < 2147483647)
			return (int32_t) (v + 0.5);
		else return 2147483647;
	}
	else
	{
		if (v < -2147483648)
			return (int32_t) (v - 0.5);
		else return -2147483648;
	}
}

uint32_t mpeg4$define(inner, f2i, uint32_t)(double v)
{
	if (v >= 0)
	{
		if (v < 4294967295)
			return (uint32_t) (v + 0.5);
		else return 4294967295;
	}
	else return 0;
}
