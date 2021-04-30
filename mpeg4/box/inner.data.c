#include "inner.data.h"
#include <stdlib.h>
#include <string.h>

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

char** mpeg4$define(inner, string, set_with_length)(char **restrict p_string, uintptr_t *restrict p_length, const char *restrict string, uintptr_t n)
{
	register char *restrict r = NULL;
	if (n)
	{
		if (!(r = (char *) malloc(n + 1)))
			goto label_fail;
		memcpy(r, string, n);
		r[n] = 0;
	}
	if (*p_string) free(*p_string);
	*p_string = r;
	if (p_length) *p_length = n;
	return p_string;
	label_fail:
	return NULL;
}

char** mpeg4$define(inner, string, set)(char **restrict p_string, uintptr_t *restrict p_length, const char *restrict string)
{
	return mpeg4$define(inner, string, set_with_length)(p_string, p_length, string, string?strlen(string):0);
}

void mpeg4$define(inner, array, init)(inner_array_t *restrict array, uintptr_t item_size)
{
	*(uintptr_t *) &array->item_size = item_size;
}

void mpeg4$define(inner, array, clear)(inner_array_t *restrict array)
{
	array->number = 0;
	array->size = 0;
	if (array->array)
	{
		free(array->array);
		array->array = NULL;
	}
}

void* mpeg4$define(inner, array, append_point)(inner_array_t *restrict array, uintptr_t n)
{
	uintptr_t pos, need, size;
	uint8_t *restrict a;
	if ((need = (pos = array->number) + n) <= (size = array->size))
	{
		label_ok:
		array->number = need;
		return (void *) (array->array + pos * array->item_size);
	}
	if (!size) size = 16;
	while (size < need)
	{
		if (size & 0x80000000u)
			goto label_fail;
		size <<= 1;
	}
	if (size > array->size)
	{
		if (!array->array)
		{
			if (!(a = (uint8_t *) calloc(size, array->item_size)))
				goto label_fail;
		}
		else
		{
			if (!(a = (uint8_t *) realloc(array->array, size * array->item_size)))
				goto label_fail;
			memset(a + array->size * array->item_size, 0, array->size * array->item_size);
		}
		array->array = a;
		array->size = size;
		goto label_ok;
	}
	label_fail:
	return NULL;
}

void* mpeg4$define(inner, array, append_data)(inner_array_t *restrict array, uintptr_t n, const void *data)
{
	void *dst;
	if ((dst = mpeg4$define(inner, array, append_point)(array, n)))
	{
		if (n) memcpy(dst, data, n * array->item_size);
		return dst;
	}
	return NULL;
}
